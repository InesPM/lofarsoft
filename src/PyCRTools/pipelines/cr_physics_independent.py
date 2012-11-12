"""CR pipeline.
"""

import logging

# Log everything, and send it to stderr.
logging.basicConfig(level=logging.DEBUG)

import matplotlib
matplotlib.use("Agg")

import sys
import time
import pytmf
import numpy as np
import pycrtools as cr
from pycrtools import crdatabase as crdb
from pycrtools import metadata as md
from pycrtools import tools
from pycrtools import lora

from optparse import OptionParser

# Don't write output to all tasks
cr.tasks.task_write_parfiles = False

# Parse commandline options
parser = OptionParser()
parser.add_option("-i", "--id", type="int", help="event ID", default=1)
parser.add_option("-b", "--blocksize", type="int", default=2**16)
parser.add_option("-d", "--database", default="cr.db", help="filename of database")
parser.add_option("-o", "--output-dir", default="./", help="output directory")
parser.add_option("-s", "--station", action="append", help="only process given station")
parser.add_option("-a", "--accept_snr", default = 5, help="accept pulses with snr higher than this in the beamformed timeseries")
parser.add_option("--maximum_nof_iterations", default = 5, help="maximum number of iterations in antenna pattern unfolding loop")
parser.add_option("--maximum_angular_diff", default = 0.5, help="maximum angular difference in direction fit iteration (in degrees), corresponds to angular resolution of a LOFAR station")
parser.add_option("--broad_search_window_width", default = 2**12, help="width of window around expected location for first pulse search")
parser.add_option("--narrow_search_window_width", default = 2**7, help="width of window around expected location for subsequent pulse search")
parser.add_option("-l", "--lora_directory", default="./", help="directory containing LORA information")
parser.add_option("--lora_logfile", default="LORAtime4", help="name of LORA logfile with timestamps")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename)
db = dbManager.db

start = time.clock()

# Get event from database
event = crdb.Event(db = db, id = options.id)

# Set the event status
event.status = "CR_PIPELINE_PROCESSING"
event.write(recursive=False, parameters=False)

cr_found = False

# Create FFTW plans
fftplan = cr.FFTWPlanManyDftR2c(options.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
invfftplan = cr.FFTWPlanManyDftC2r(options.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

# Loop over all stations in event
stations = []
for f in event.datafiles:
    stations.extend(f.stations)

combined_antenna_positions = []
combined_pulse_peak_amplitude = []
combined_rms = []

# Process only given stations if explicitly requested
for station in stations:

    if options.station and station.stationname not in options.station:
        continue

    logging.debug("processing station "+station.stationname)

    try:

        # Open file
        f = cr.open(station.datafile.settings.datapath+'/'+station.datafile.filename)

        # Read LORA information
        tbb_time = f["TIME"][0]
        tbb_sample_number = max(f["SAMPLE_NUMBER"])
        
        try:
            logging.debug("reading LORA data")
        
            (tbb_time_sec, tbb_time_nsec) = lora.nsecFromSec(tbb_time, logfile = options.lora_directory + options.lora_logfile)

            (block_number_lora, sample_number_lora) = lora.loraTimestampToBlocknumber(tbb_time_sec, tbb_time_nsec, tbb_time, tbb_sample_number, blocksize = options.blocksize)

            pulse_search_window_start = sample_number_lora - options.broad_search_window_width / 2
            pulse_search_window_end = sample_number_lora + options.broad_search_window_width / 2

            print "look for pulse between sample {0:d} and {1:d} in block {2:d}".format(pulse_search_window_start, pulse_search_window_end, block_number_lora)
        except Exception:
        
            logging.exception("could not get expected block number from LORA data for station "+station.stationname)
        
            continue
        
        logging.debug("have LORA data")

        # Set file parameters to match LORA block
        f["BLOCKSIZE"] = options.blocksize
        f["BLOCK"] = block_number_lora

        # Find RFI and bad antennas
        findrfi = cr.trun("FindRFI", f = f, plot_prefix = options.output_dir+"/"+"cr_physics-"+station.stationname+"-"+str(options.id)+"-")

        # Select antennas which are marked good for both polarization
        dipole_names = f["DIPOLE_NAMES"]

        selected_dipoles = []
        for i in range(len(dipole_names)/2):
            if dipole_names[2*i] in findrfi.good_antennas and dipole_names[2*i+1] in findrfi.good_antennas:
                selected_dipoles.extend([dipole_names[2*i], dipole_names[2*i+1]])

        f["SELECTED_DIPOLES"] = selected_dipoles

        # Read FFT data
        fft_data = f.empty("FFT_DATA")
        f.getFFTData(fft_data, block_number_lora, False)
        frequencies = cr.hArray(f["FREQUENCY_DATA"])

        # Flag dirty channels (from RFI excission)
#        fft_data[..., findrfi.dirty_channels] = 0
        fft_data[..., [0, ]] = 0

        # Apply calibration delays
        try:
            cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
        except:
            print "Error when obtaining DIPOLE_CALIBRATION_DELAY skipping station", f["STATION_NAME"]

        weights = cr.hArray(complex, dimensions=fft_data, name="Complex Weights")
        phases = cr.hArray(float, dimensions=fft_data, name="Phases", xvalues=frequencies)

        cr.hDelayToPhase(phases, frequencies, cabledelays)
        cr.hPhaseToComplex(weights, phases)

        fft_data.mul(weights)

        # Get expected galactic noise strength
        galactic_noise = cr.trun("GalacticNoise", timestamp = tbb_time)

#        # Get measured noise strength (using very ugly code needed because not all dipoles are selected and results are stored per polarization)
#        antennas_spectral_power = dict(zip(
#            station.polarization['0']["antennas"].values()+station.polarization['1']["antennas"].values(),
#            station.polarization['0']["antennas_spectral_power"]+station.polarization['1']["antennas_spectral_power"]
#            ))
#
#        antennas_spectral_power_correction = cr.hArray([antennas_spectral_power[k] for k in selected_dipoles])
#
#        # Correct to expected level
#        cr.hInverse(antennas_spectral_power_correction)
#        cr.hMul(antennas_spectral_power_correction, galactic_noise.galactic_noise)
#        cr.hMul(fft_data[...], antennas_spectral_power_correction[...])

        # Get timeseries data
        timeseries_data = f.empty("TIMESERIES_DATA")
        nantennas = timeseries_data.shape()[0] / 2

        # Get antennas positions
        antenna_positions = f["ANTENNA_POSITIONS"]

        # Get first estimate of pulse direction
        pulse_direction = list(event["lora_direction"])

        # Beamform in LORA direction for both polarizations
        fft_data_0 = cr.hArray(complex, dimensions = (nantennas, options.blocksize / 2 + 1))
        fft_data_1 = cr.hArray(complex, dimensions = (nantennas, options.blocksize / 2 + 1))

        fft_data_0[...].copy(fft_data[0:nantennas:2,...])
        fft_data_1[...].copy(fft_data[1:nantennas:2,...])

        antenna_positions_one = cr.hArray(float, dimensions = (nantennas, 3))
        antenna_positions_one[...].copy(antenna_positions[0:nantennas:3,...])

        mb0 = cr.trun("MiniBeamformer", fft_data = fft_data_0, frequencies = frequencies, antpos = antenna_positions_one, direction = pulse_direction)
        mb1 = cr.trun("MiniBeamformer", fft_data = fft_data_1, frequencies = frequencies, antpos = antenna_positions_one, direction = pulse_direction)

        beamformed_timeseries = cr.hArray(float, dimensions = (2, options.blocksize))

        cr.hFFTWExecutePlan(beamformed_timeseries[0], mb0.beamformed_fft, invfftplan)
        cr.hFFTWExecutePlan(beamformed_timeseries[1], mb1.beamformed_fft, invfftplan)

        beamformed_timeseries /= options.blocksize

        # Look for significant pulse in beamformed signal
        pulse_envelope_bf = cr.trun("PulseEnvelope", timeseries_data = beamformed_timeseries, pulse_start = pulse_search_window_start, pulse_end = pulse_search_window_end, nsigma = options.accept_snr, save_plots = True, plot_prefix = options.output_dir+"/"+"cr_physics-"+station.stationname+"-"+str(options.id)+"-bf-", plotlist = [])

        p0 = station.polarization['0']
        p1 = station.polarization['1']

        p0["plotfiles"] = ["/"+s.lstrip("./") for s in [pulse_envelope_bf.plotlist[0], ]]
        p1["plotfiles"] = ["/"+s.lstrip("./") for s in [pulse_envelope_bf.plotlist[1], ]]

        cr_found_in_station = False
        if 0 in pulse_envelope_bf.antennas_with_significant_pulses:
            cr_found_in_station = True
            p0.status = "GOOD"
        else:
            p0.status = "BAD"

        if 1 in pulse_envelope_bf.antennas_with_significant_pulses:
            cr_found_in_station = True
            p1.status = "GOOD"
        else:
            p1.status = "BAD"

        # skip this station for further processing when no cosmic ray signal is found in the beamformed timeseries
        # in the LORA direction for at least one of the polarizations
        if cr_found_in_station:
            cr_found = True
        else:
            continue

        # Get pulse window
        pulse_start = pulse_search_window_start + int(pulse_envelope_bf.meanpos) - max(options.narrow_search_window_width / 2, pulse_envelope_bf.maxdiff / 2)
        pulse_end = pulse_search_window_start + int(pulse_envelope_bf.meanpos) + max(options.narrow_search_window_width / 2, pulse_envelope_bf.maxdiff / 2)

        print "now looking for pulse in narrow range between samples {0:d} and {1:d}".format(pulse_start, pulse_end)

        # Start direction fitting loop
        n = 0
        direction_fit_converged = False
        while True:

            # Unfold antenna pattern
            antenna_response = cr.trun("AntennaResponse", instrumental_polarization = fft_data, frequencies = frequencies, direction = pulse_direction)

            # Get timeseries data
            cr.hFFTWExecutePlan(timeseries_data[...], antenna_response.on_sky_polarization[...], invfftplan)
            timeseries_data /= options.blocksize

            # Calculate delays
            pulse_envelope = cr.trun("PulseEnvelope", timeseries_data = timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)

            # Use current direction if not enough significant pulses are found for direction fitting
            if len(pulse_envelope.antennas_with_significant_pulses) < 3:
                print "not enough antennas with significant pulses, using previous direction"
                break
            
            # Fit pulse direction
            direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions = antenna_positions, timelags = pulse_envelope.delays, good_antennas = pulse_envelope.antennas_with_significant_pulses,reference_antenna = pulse_envelope.refant, verbose=True)

            pulse_direction = direction_fit_plane_wave.meandirection_azel_deg

            # Check for convergence of iterative direction fitting loop
            if n > 0:
                angular_diff = np.rad2deg(tools.spaceAngle( np.deg2rad((90-last_direction[1])), np.deg2rad((90-last_direction[0])), np.deg2rad((90-pulse_direction[1])), np.deg2rad((90-pulse_direction[0]))))
                
                if angular_diff < options.maximum_angular_diff:
                    direction_fit_converged = True

            last_direction = pulse_direction

            n += 1
            if direction_fit_converged:
                print "fit converged"
                station["crp_pulse_direction"] = pulse_direction
                break

            if n > options.maximum_nof_iterations:
                print "maximum number of iterations reached"
                station["crp_pulse_direction"] = pulse_direction
                break

            if direction_fit_plane_wave.fit_failed:
                print "direction fit failed"
                break

        # Project polarization onto x,y,z frame
        xyz_timeseries_data = cr.hArray(float, dimensions = (3*nantennas, options.blocksize))
        cr.hProjectPolarizations(xyz_timeseries_data[0:3*nantennas:3,...], xyz_timeseries_data[1:3*nantennas:3,...], xyz_timeseries_data[2:3*nantennas:3,...], timeseries_data[0:2*nantennas:2,...], timeseries_data[1:2*nantennas:2,...], pytmf.deg2rad(pulse_direction[0]), pytmf.deg2rad(pulse_direction[1]))

        # Get Stokes parameters
        stokes_parameters = cr.trun("StokesParameters", timeseries_data = xyz_timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)

        # Get pulse strength
        pulse_envelope_xyz = cr.trun("PulseEnvelope", timeseries_data = xyz_timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10, save_plots = True, plot_prefix = options.output_dir+"/"+"cr_physics-"+station.stationname+"-"+str(options.id)+"-", plotlist = [])

        # Do noise characterization
        noise = cr.trun("Noise", timeseries_data = xyz_timeseries_data, plot_prefix = options.output_dir+"/"+"cr_physics-"+station.stationname+"-"+str(options.id)+"-")

        # Calculate time delay of pulse with respect to the start time of the file (e.g. f["TIME"])
        time_delays = pulse_envelope_xyz.pulse_maximum_time.toNumpy().reshape((nantennas,3))
        time_delays += float(block_number_lora * options.blocksize + max(f["SAMPLE_NUMBER"])) / f["SAMPLE_FREQUENCY"][0] + f["CLOCK_OFFSET"][0]

        # Get xyz-polarization instance
        p = station.polarization['xyz']

        # Add parameters
        p["crp_itrf_antenna_positions"] = md.convertITRFToLocal(f["ITRFANTENNA_POSITIONS"]).toNumpy()
        p["crp_pulse_delays"] = time_delays
        p["crp_pulse_peak_amplitude"] = cr.hArray(pulse_envelope_xyz.peak_amplitude).toNumpy().reshape((nantennas, 3))
        p["crp_rms"] = cr.hArray(pulse_envelope_xyz.rms).toNumpy().reshape((nantennas, 3))
        p["crp_stokes"] = stokes_parameters.stokes.toNumpy()
        p["crp_polarization_angle"] = stokes_parameters.polarization_angle.toNumpy()
        p["plotfiles"] = ["/"+s.lstrip("./") for s in pulse_envelope_xyz.plotlist + noise.plotlist]

        p.status = "GOOD"
        p.write()

    except Exception:

        logging.exception("unexpected error occured when processing station "+station.stationname)

        p = station.polarization['xyz']

        p.status = "BAD"
        p.write()

    logging.debug("finishing station "+station.stationname)

# Create list of event level plots
plotlist = []

# Get combined parameters from (cached) database
all_station_antenna_positions = []
all_station_pulse_delays = []
all_station_pulse_peak_amplitude = []
all_station_rms = []
all_station_direction = []

for station in stations:
    try:
        all_station_direction.append(station["crp_pulse_direction"])
        p = station.polarization["xyz"]
        all_station_antenna_positions.append(p["crp_itrf_antenna_positions"])
        all_station_pulse_delays.append(p["crp_pulse_delays"])
        all_station_pulse_peak_amplitude.append(p["crp_pulse_peak_amplitude"])
        all_station_rms.append(p["crp_rms"])
    except:
        print "Do not have all pulse parameters for station", station.stationname

all_station_antenna_positions = np.vstack(all_station_antenna_positions)
all_station_pulse_delays = np.vstack(all_station_pulse_delays)
all_station_pulse_peak_amplitude = np.vstack(all_station_pulse_peak_amplitude)
all_station_rms = np.vstack(all_station_rms)
all_station_direction = np.asarray(all_station_direction)

# Convert to contiguous array of correct shape
shape = all_station_antenna_positions.shape
all_station_antenna_positions = all_station_antenna_positions.reshape((shape[0] / 2, 2, shape[1]))[:,0]
all_station_antenna_positions = all_station_antenna_positions.copy()

# Calculate average direction and store it
average_direction = tools.averageDirectionLOFAR(all_station_direction[:,0], all_station_direction[:,1])
event["crp_average_direction"] = average_direction

# Beamform with all stations

# Compute LDF and footprint
core = list(event["lora_core"])
core_uncertainties = event["lora_coreuncertainties"].toNumpy()
direction_uncertainties = [3.,3.,0]

ldf = cr.trun("Shower", positions = all_station_antenna_positions, signals_uncertainties = all_station_rms, core = core, direction = average_direction, timelags = all_station_pulse_delays, core_uncertainties = core_uncertainties, signals = all_station_pulse_peak_amplitude, direction_uncertainties = direction_uncertainties, ldf_enable = True, footprint_enable = True, save_plots = True, plot_prefix = options.output_dir+"/"+"cr_physics-"+str(options.id)+"-")

# Add LDF and footprint plots to list of event level plots
plotlist.extend(ldf.plotlist)

# Add list of event level plots to event
event["plotfiles"] = ["/"+p.lstrip("./") for p in plotlist]

# Update event status
if cr_found:
    event.status = "CR_FOUND"
else:
    event.status = "CR_NOT_FOUND"
event.write()

print "[cr_physics] completed in {0:.3f} s".format(time.clock() - start)

