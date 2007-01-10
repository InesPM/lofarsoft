
// include of header files 

#include <casa/Arrays/Array.h>
#include <casa/Arrays/MaskedArray.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicMath/Random.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/Logging/LogIO.h>
#include <casa/Quanta/MVDirection.h>
#include <casa/Quanta/MVFrequency.h>
#include <casa/Quanta/MVPosition.h>
#include <casa/Quanta/MVRadialVelocity.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Quanta/QuantumHolder.h>
#include <casa/Quanta/Unit.h>
#include <casa/Utilities/CountedPtr.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIO.cc>
#include <casa/Arrays/ArrayIter.cc>
#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/ArrayUtil.cc>
#include <casa/Arrays/Cube.cc>
#include <casa/Arrays/MaskArrMath.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Matrix.cc>
#include <casa/Arrays/MatrixIter.cc>
#include <casa/Arrays/MatrixMath.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Arrays/Vector2.cc>
#include <casa/Arrays/VectorIter.cc>

namespace casa {
  
  template class Array<Slicer>;
  template class Array<Vector<Complex> >;
  template class Array<Vector<Double> >;
  template class Array<Vector<Float> >;
  template class Array<Complex>;
  template class Array<DComplex>;
  template class Array<String>;
  template class Array<MVDirection>;
  template class Array<MVFrequency>;
  template class Array<MVPosition>;
  template class Array<MVRadialVelocity>;
  template class Array<MVTime>;
  template class Array<Quantum<Double> >;
  template class Array<Quantum<Float> >;
  template class Array<QuantumHolder>;
  template class Array<Unit>;
  template class Array<Matrix<CountedPtr<Random> > >;
  template class Array<CountedPtr<Random> >;
  template class Array<Array<Double> >;
  template class Array<Double>;
  template class Array<Float *>;
  template class Array<Float>;
  template class Array<uLong>;
  template class Array<Long>;
  template class Array<uInt>;
  template class Array<Int>;
  template class Array<uShort>;
  template class Array<Short>;
  template class Array<uChar>;
  template class Array<Char>;
  template class Array<Bool>;
  
#ifdef AIPS_SUN_NATIVE 
  template class Array<Vector<Complex> >::ConstIteratorSTL;
  template class Array<Vector<Double> >::ConstIteratorSTL;
  template class Array<Complex>::ConstIteratorSTL;
  template class Array<DComplex>::ConstIteratorSTL;
  template class Array<String>::ConstIteratorSTL;
  template class Array<MVDirection>::ConstIteratorSTL;
  template class Array<MVFrequency>::ConstIteratorSTL;
  template class Array<MVPosition>::ConstIteratorSTL;
  template class Array<MVRadialVelocity>::ConstIteratorSTL;
  template class Array<MVTime>::ConstIteratorSTL;
  template class Array<Quantum<Double> >::ConstIteratorSTL;
  template class Array<Quantum<Float> >::ConstIteratorSTL;
  template class Array<QuantumHolder>::ConstIteratorSTL 
  template class Array<Unit>::ConstIteratorSTL;
  template class Array<Bool>::ConstIteratorSTL;
  template class Array<Char>::ConstIteratorSTL;
  template class Array<Double>::ConstIteratorSTL;
  template class Array<Float *>::ConstIteratorSTL;
  template class Array<Float>::ConstIteratorSTL;
  template class Array<Int>::ConstIteratorSTL;
  template class Array<Long>::ConstIteratorSTL;
  template class Array<Short>::ConstIteratorSTL;
  template class Array<uChar>::ConstIteratorSTL;
  template class Array<uInt>::ConstIteratorSTL;
  template class Array<uLong>::ConstIteratorSTL;
  template class Array<uShort>::ConstIteratorSTL;
#endif 
  
  template AipsIO & operator<<(AipsIO &, Array<Bool> const &);
  template AipsIO & operator<<(AipsIO &, Array<uChar> const &);
  template AipsIO & operator<<(AipsIO &, Array<Short> const &);
  template AipsIO & operator<<(AipsIO &, Array<Int> const &);
  template AipsIO & operator<<(AipsIO &, Array<uInt> const &);
  template AipsIO & operator<<(AipsIO &, Array<Float> const &);
  template AipsIO & operator<<(AipsIO &, Array<Double> const &);
  template AipsIO & operator<<(AipsIO &, Array<Complex> const &);
  template AipsIO & operator<<(AipsIO &, Array<DComplex> const &);
  template AipsIO & operator<<(AipsIO &, Array<String> const &);
  template void putArray(AipsIO &, Array<Bool> const &, Char const *);
  template void putArray(AipsIO &, Array<uChar> const &, Char const *);
  template void putArray(AipsIO &, Array<Short> const &, Char const *);
  template void putArray(AipsIO &, Array<Int> const &, Char const *);
  template void putArray(AipsIO &, Array<uInt> const &, Char const *);
  template void putArray(AipsIO &, Array<Float> const &, Char const *);
  template void putArray(AipsIO &, Array<Double> const &, Char const *);
  template void putArray(AipsIO &, Array<Complex> const &, Char const *);
  template void putArray(AipsIO &, Array<DComplex> const &, Char const *);
  template void putArray(AipsIO &, Array<String> const &, Char const *);
  template AipsIO & operator>>(AipsIO &, Array<Bool> &);
  template AipsIO & operator>>(AipsIO &, Array<uChar> &);
  template AipsIO & operator>>(AipsIO &, Array<Short> &);
  template AipsIO & operator>>(AipsIO &, Array<Int> &);
  template AipsIO & operator>>(AipsIO &, Array<uInt> &);
  template AipsIO & operator>>(AipsIO &, Array<Float> &);
  template AipsIO & operator>>(AipsIO &, Array<Double> &);
  template AipsIO & operator>>(AipsIO &, Array<Complex> &);
  template AipsIO & operator>>(AipsIO &, Array<DComplex> &);
  template AipsIO & operator>>(AipsIO &, Array<String> &);
  template ostream & operator<<(ostream &, Array<Complex> const &);
  template ostream & operator<<(ostream &, Array<DComplex> const &);
  template LogIO & operator<<(LogIO &, Array<Complex> const &);
  template LogIO & operator<<(LogIO &, Array<DComplex> const &);
  template LogIO & operator<<(LogIO &, Array<String> const &);
  template LogIO & operator<<(LogIO &, Array<Double> const &);
  template LogIO & operator<<(LogIO &, Array<Float> const &);
  template LogIO & operator<<(LogIO &, Array<Int> const &);
  template LogIO & operator<<(LogIO &, Array<uInt> const &);
  template LogIO & operator<<(LogIO &, Array<Short> const &);
  template LogIO & operator<<(LogIO &, Array<uChar> const &);
  template LogIO & operator<<(LogIO &, Array<Bool> const &);
  template ostream & operator<<(ostream &, Array<Quantum<Double> > const &);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<uInt> &, IPosition const *, Bool);
  template istream & operator>>(istream &, Array<String> &);
  template Bool read(istream &, Array<String> &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<String> &, IPosition const *, Bool);
  template ostream & operator<<(ostream &, Array<Bool> const &);
  template ostream & operator<<(ostream &, Array<Char> const &);
  template ostream & operator<<(ostream &, Array<Double> const &);
  template ostream & operator<<(ostream &, Array<Float> const &);
  template ostream & operator<<(ostream &, Array<Int> const &);
  template ostream & operator<<(ostream &, Array<Long> const &);
  template ostream & operator<<(ostream &, Array<Short> const &);
  template ostream & operator<<(ostream &, Array<String> const &);
  template ostream & operator<<(ostream &, Array<uChar> const &);
  template ostream & operator<<(ostream &, Array<uInt> const &);
  template ostream & operator<<(ostream &, Array<uLong> const &);
  template ostream & operator<<(ostream &, Array<uShort> const &);
  // test/templates
  template Bool read(istream &, Array<Complex> &, IPosition const *, Bool);
  template Bool read(istream &, Array<Double> &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<Complex> &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<Double> &, IPosition const *, Bool);
  template istream & operator>>(istream &, Array<Double> &);
  template void readAsciiMatrix(Matrix<Int> &, Char const *);
  template void readAsciiVector(Vector<Double> &, Char const *);
  template void read_array(Array<Int> &, Char const *);
  template void read_array(Array<Int> &, String const &);
  template void writeAsciiMatrix(Matrix<Int> const &, Char const *);
  template void writeAsciiVector(Vector<Double> const &, Char const *);
  template void write_array(Array<Int> const &, Char const *);
  template void write_array(Array<Int> const &, String const &);
  template istream & operator>>(istream &, Array<Quantum<Double> > &);
  template Bool read(istream &, Array<Quantum<Double> > &, IPosition const *, Bool);
  template Bool readArrayBlock(istream &, Bool &, IPosition &, Block<Quantum<Double> > &, IPosition const *, Bool);
  
  template class ArrayIterator<Bool>;
  template class ArrayIterator<uChar>;
  template class ArrayIterator<Short>;
  template class ArrayIterator<uShort>;
  template class ArrayIterator<Int>;
  template class ArrayIterator<uInt>;
  template class ArrayIterator<Float>;
  template class ArrayIterator<Double>;
  template class ArrayIterator<Complex>;
  template class ArrayIterator<DComplex>;
  template class ArrayIterator<String>;
  template class ReadOnlyArrayIterator<Bool>;
  template class ReadOnlyArrayIterator<uChar>;
  template class ReadOnlyArrayIterator<Short>;
  template class ReadOnlyArrayIterator<uShort>;
  template class ReadOnlyArrayIterator<Int>;
  template class ReadOnlyArrayIterator<uInt>;
  template class ReadOnlyArrayIterator<Float>;
  template class ReadOnlyArrayIterator<Double>;
  template class ReadOnlyArrayIterator<Complex>;
  template class ReadOnlyArrayIterator<DComplex>;
  template class ReadOnlyArrayIterator<String>;
  template class ArrayIterator<Char>;
  template class ArrayIterator<uLong>;
  template class ReadOnlyArrayIterator<Char>;

  template class MatrixIterator<Bool>;
  template class MatrixIterator<uChar>;
  template class MatrixIterator<Short>;
  template class MatrixIterator<uShort>;
  template class MatrixIterator<Int>;
  template class MatrixIterator<uInt>;
  template class MatrixIterator<Float>;
  template class MatrixIterator<Double>;
  template class MatrixIterator<Complex>;
  template class MatrixIterator<DComplex>;
  template class MatrixIterator<String>;

  // allEQ

  template bool allEQ(Bool const&, Array<Bool> const&);
  template bool allEQ(Int const&, Array<Int> const&);
  template bool allEQ(uInt const&, Array<uInt> const&);
  template bool allEQ(Short const&, Array<Short> const&);
  template bool allEQ(uShort const&, Array<uShort> const&);
  template bool allEQ(Long const&, Array<Long> const&);
  template bool allEQ(uLong const&, Array<uLong> const&);
  template bool allEQ(Float const&, Array<Float> const&);
  template bool allEQ(Double const&, Array<Double> const&);
  template bool allEQ(Complex const&, Array<Complex> const&);
  template bool allEQ(DComplex const&, Array<DComplex> const&);
  template bool allEQ(String const&, Array<String> const&);

  template Bool allEQ(Array<Bool> const &, Bool const &);
  template Bool allEQ(Array<Int> const &, Int const &);
  template Bool allEQ(Array<uInt> const &, uInt const &);
  template Bool allEQ(Array<Short> const &, Short const &);
  template Bool allEQ(Array<uShort> const &, uShort const &);
  template Bool allEQ(Array<Long> const &, Long const &);
  template Bool allEQ(Array<uLong> const &, uLong const &);
  template Bool allEQ(Array<float> const &, float const &);
  template Bool allEQ(Array<double> const &, double const &);
  template Bool allEQ(Array<Complex> const &, Complex const &);
  template Bool allEQ(Array<DComplex> const &, DComplex const &);
  template Bool allEQ(Array<String> const &, String const &);

  template Bool allEQ(Array<bool> const &, Array<bool> const &);
  template Bool allEQ(Array<Int> const &, Array<Int> const &);
  template Bool allEQ(Array<uInt> const &, Array<uInt> const &);
  template Bool allEQ(Array<Short> const &, Array<Short> const &);
  template Bool allEQ(Array<uShort> const &, Array<uShort> const &);
  template Bool allEQ(Array<Long> const &, Array<Long> const &);
  template Bool allEQ(Array<uLong> const &, Array<uLong> const &);
  template Bool allEQ(Array<Float> const &, Array<Float> const &);
  template Bool allEQ(Array<Double> const &, Array<Double> const &);
  template Bool allEQ(Array<Complex> const &, Array<Complex> const &);
  template Bool allEQ(Array<DComplex> const &, Array<DComplex> const &);
  template Bool allEQ(Array<String> const &, Array<String> const &);

  // allGE

  template bool allGE(Bool const&, Array<Bool> const&);
  template bool allGE(Int const&, Array<Int> const&);
  template bool allGE(uInt const&, Array<uInt> const&);
  template bool allGE(Short const&, Array<Short> const&);
  template bool allGE(uShort const&, Array<uShort> const&);
  template bool allGE(Long const&, Array<Long> const&);
  template bool allGE(uLong const&, Array<uLong> const&);
  template bool allGE(Float const&, Array<Float> const&);
  template bool allGE(Double const&, Array<Double> const&);
  template bool allGE(Complex const&, Array<Complex> const&);
  template bool allGE(DComplex const&, Array<DComplex> const&);
  template bool allGE(String const&, Array<String> const&);

  template Bool allGE(Array<Bool> const &, Bool const &);
  template Bool allGE(Array<Int> const &, Int const &);
  template Bool allGE(Array<uInt> const &, uInt const &);
  template Bool allGE(Array<Short> const &, Short const &);
  template Bool allGE(Array<uShort> const &, uShort const &);
  template Bool allGE(Array<Long> const &, Long const &);
  template Bool allGE(Array<uLong> const &, uLong const &);
  template Bool allGE(Array<float> const &, float const &);
  template Bool allGE(Array<double> const &, double const &);
  template Bool allGE(Array<Complex> const &, Complex const &);
  template Bool allGE(Array<DComplex> const &, DComplex const &);
  template Bool allGE(Array<String> const &, String const &);

  template Bool allGE(Array<bool> const &, Array<bool> const &);
  template Bool allGE(Array<Int> const &, Array<Int> const &);
  template Bool allGE(Array<uInt> const &, Array<uInt> const &);
  template Bool allGE(Array<Short> const &, Array<Short> const &);
  template Bool allGE(Array<uShort> const &, Array<uShort> const &);
  template Bool allGE(Array<Long> const &, Array<Long> const &);
  template Bool allGE(Array<uLong> const &, Array<uLong> const &);
  template Bool allGE(Array<Float> const &, Array<Float> const &);
  template Bool allGE(Array<Double> const &, Array<Double> const &);
  template Bool allGE(Array<Complex> const &, Array<Complex> const &);
  template Bool allGE(Array<DComplex> const &, Array<DComplex> const &);
  template Bool allGE(Array<String> const &, Array<String> const &);

  // allGT

  template bool allGT(Bool const&, Array<Bool> const&);
  template bool allGT(Int const&, Array<Int> const&);
  template bool allGT(uInt const&, Array<uInt> const&);
  template bool allGT(Short const&, Array<Short> const&);
  template bool allGT(uShort const&, Array<uShort> const&);
  template bool allGT(Long const&, Array<Long> const&);
  template bool allGT(uLong const&, Array<uLong> const&);
  template bool allGT(Float const&, Array<Float> const&);
  template bool allGT(Double const&, Array<Double> const&);
  template bool allGT(Complex const&, Array<Complex> const&);
  template bool allGT(DComplex const&, Array<DComplex> const&);
  template bool allGT(String const&, Array<String> const&);

  template Bool allGT(Array<Bool> const &, Bool const &);
  template Bool allGT(Array<Int> const &, Int const &);
  template Bool allGT(Array<uInt> const &, uInt const &);
  template Bool allGT(Array<Short> const &, Short const &);
  template Bool allGT(Array<uShort> const &, uShort const &);
  template Bool allGT(Array<Long> const &, Long const &);
  template Bool allGT(Array<uLong> const &, uLong const &);
  template Bool allGT(Array<float> const &, float const &);
  template Bool allGT(Array<double> const &, double const &);
  template Bool allGT(Array<Complex> const &, Complex const &);
  template Bool allGT(Array<DComplex> const &, DComplex const &);
  template Bool allGT(Array<String> const &, String const &);

  template Bool allGT(Array<bool> const &, Array<bool> const &);
  template Bool allGT(Array<Int> const &, Array<Int> const &);
  template Bool allGT(Array<uInt> const &, Array<uInt> const &);
  template Bool allGT(Array<Short> const &, Array<Short> const &);
  template Bool allGT(Array<uShort> const &, Array<uShort> const &);
  template Bool allGT(Array<Long> const &, Array<Long> const &);
  template Bool allGT(Array<uLong> const &, Array<uLong> const &);
  template Bool allGT(Array<Float> const &, Array<Float> const &);
  template Bool allGT(Array<Double> const &, Array<Double> const &);
  template Bool allGT(Array<Complex> const &, Array<Complex> const &);
  template Bool allGT(Array<DComplex> const &, Array<DComplex> const &);
  template Bool allGT(Array<String> const &, Array<String> const &);

  // allLE

  template bool allLE(Bool const&, Array<Bool> const&);
  template bool allLE(Int const&, Array<Int> const&);
  template bool allLE(uInt const&, Array<uInt> const&);
  template bool allLE(Short const&, Array<Short> const&);
  template bool allLE(uShort const&, Array<uShort> const&);
  template bool allLE(Long const&, Array<Long> const&);
  template bool allLE(uLong const&, Array<uLong> const&);
  template bool allLE(Float const&, Array<Float> const&);
  template bool allLE(Double const&, Array<Double> const&);
  template bool allLE(Complex const&, Array<Complex> const&);
  template bool allLE(DComplex const&, Array<DComplex> const&);
  template bool allLE(String const&, Array<String> const&);

  template Bool allLE(Array<Bool> const &, Bool const &);
  template Bool allLE(Array<Int> const &, Int const &);
  template Bool allLE(Array<uInt> const &, uInt const &);
  template Bool allLE(Array<Short> const &, Short const &);
  template Bool allLE(Array<uShort> const &, uShort const &);
  template Bool allLE(Array<Long> const &, Long const &);
  template Bool allLE(Array<uLong> const &, uLong const &);
  template Bool allLE(Array<float> const &, float const &);
  template Bool allLE(Array<double> const &, double const &);
  template Bool allLE(Array<Complex> const &, Complex const &);
  template Bool allLE(Array<DComplex> const &, DComplex const &);
  template Bool allLE(Array<String> const &, String const &);

  template Bool allLE(Array<bool> const &, Array<bool> const &);
  template Bool allLE(Array<Int> const &, Array<Int> const &);
  template Bool allLE(Array<uInt> const &, Array<uInt> const &);
  template Bool allLE(Array<Short> const &, Array<Short> const &);
  template Bool allLE(Array<uShort> const &, Array<uShort> const &);
  template Bool allLE(Array<Long> const &, Array<Long> const &);
  template Bool allLE(Array<uLong> const &, Array<uLong> const &);
  template Bool allLE(Array<Float> const &, Array<Float> const &);
  template Bool allLE(Array<Double> const &, Array<Double> const &);
  template Bool allLE(Array<Complex> const &, Array<Complex> const &);
  template Bool allLE(Array<DComplex> const &, Array<DComplex> const &);
  template Bool allLE(Array<String> const &, Array<String> const &);

  // allLT

  template bool allLT(Bool const&, Array<Bool> const&);
  template bool allLT(Int const&, Array<Int> const&);
  template bool allLT(uInt const&, Array<uInt> const&);
  template bool allLT(Short const&, Array<Short> const&);
  template bool allLT(uShort const&, Array<uShort> const&);
  template bool allLT(Long const&, Array<Long> const&);
  template bool allLT(uLong const&, Array<uLong> const&);
  template bool allLT(Float const&, Array<Float> const&);
  template bool allLT(Double const&, Array<Double> const&);
  template bool allLT(Complex const&, Array<Complex> const&);
  template bool allLT(DComplex const&, Array<DComplex> const&);
  template bool allLT(String const&, Array<String> const&);

  template Bool allLT(Array<Bool> const &, Bool const &);
  template Bool allLT(Array<Int> const &, Int const &);
  template Bool allLT(Array<uInt> const &, uInt const &);
  template Bool allLT(Array<Short> const &, Short const &);
  template Bool allLT(Array<uShort> const &, uShort const &);
  template Bool allLT(Array<Long> const &, Long const &);
  template Bool allLT(Array<uLong> const &, uLong const &);
  template Bool allLT(Array<float> const &, float const &);
  template Bool allLT(Array<double> const &, double const &);
  template Bool allLT(Array<Complex> const &, Complex const &);
  template Bool allLT(Array<DComplex> const &, DComplex const &);
  template Bool allLT(Array<String> const &, String const &);

  template Bool allLT(Array<bool> const &, Array<bool> const &);
  template Bool allLT(Array<Int> const &, Array<Int> const &);
  template Bool allLT(Array<uInt> const &, Array<uInt> const &);
  template Bool allLT(Array<Short> const &, Array<Short> const &);
  template Bool allLT(Array<uShort> const &, Array<uShort> const &);
  template Bool allLT(Array<Long> const &, Array<Long> const &);
  template Bool allLT(Array<uLong> const &, Array<uLong> const &);
  template Bool allLT(Array<Float> const &, Array<Float> const &);
  template Bool allLT(Array<Double> const &, Array<Double> const &);
  template Bool allLT(Array<Complex> const &, Array<Complex> const &);
  template Bool allLT(Array<DComplex> const &, Array<DComplex> const &);
  template Bool allLT(Array<String> const &, Array<String> const &);

  // allNE

  template bool allNE(Bool const&, Array<Bool> const&);
  template bool allNE(Int const&, Array<Int> const&);
  template bool allNE(uInt const&, Array<uInt> const&);
  template bool allNE(Short const&, Array<Short> const&);
  template bool allNE(uShort const&, Array<uShort> const&);
  template bool allNE(Long const&, Array<Long> const&);
  template bool allNE(uLong const&, Array<uLong> const&);
  template bool allNE(Float const&, Array<Float> const&);
  template bool allNE(Double const&, Array<Double> const&);
  template bool allNE(Complex const&, Array<Complex> const&);
  template bool allNE(DComplex const&, Array<DComplex> const&);
  template bool allNE(String const&, Array<String> const&);

  template Bool allNE(Array<Bool> const &, Bool const &);
  template Bool allNE(Array<Int> const &, Int const &);
  template Bool allNE(Array<uInt> const &, uInt const &);
  template Bool allNE(Array<Short> const &, Short const &);
  template Bool allNE(Array<uShort> const &, uShort const &);
  template Bool allNE(Array<Long> const &, Long const &);
  template Bool allNE(Array<uLong> const &, uLong const &);
  template Bool allNE(Array<float> const &, float const &);
  template Bool allNE(Array<double> const &, double const &);
  template Bool allNE(Array<Complex> const &, Complex const &);
  template Bool allNE(Array<DComplex> const &, DComplex const &);
  template Bool allNE(Array<String> const &, String const &);

  template Bool allNE(Array<bool> const &, Array<bool> const &);
  template Bool allNE(Array<Int> const &, Array<Int> const &);
  template Bool allNE(Array<uInt> const &, Array<uInt> const &);
  template Bool allNE(Array<Short> const &, Array<Short> const &);
  template Bool allNE(Array<uShort> const &, Array<uShort> const &);
  template Bool allNE(Array<Long> const &, Array<Long> const &);
  template Bool allNE(Array<uLong> const &, Array<uLong> const &);
  template Bool allNE(Array<Float> const &, Array<Float> const &);
  template Bool allNE(Array<Double> const &, Array<Double> const &);
  template Bool allNE(Array<Complex> const &, Array<Complex> const &);
  template Bool allNE(Array<DComplex> const &, Array<DComplex> const &);
  template Bool allNE(Array<String> const &, Array<String> const &);

  // anyEQ

  template bool anyEQ(Bool const&, Array<Bool> const&);
  template bool anyEQ(Int const&, Array<Int> const&);
  template bool anyEQ(uInt const&, Array<uInt> const&);
  template bool anyEQ(Short const&, Array<Short> const&);
  template bool anyEQ(uShort const&, Array<uShort> const&);
  template bool anyEQ(Long const&, Array<Long> const&);
  template bool anyEQ(uLong const&, Array<uLong> const&);
  template bool anyEQ(Float const&, Array<Float> const&);
  template bool anyEQ(Double const&, Array<Double> const&);
  template bool anyEQ(Complex const&, Array<Complex> const&);
  template bool anyEQ(DComplex const&, Array<DComplex> const&);
  template bool anyEQ(String const&, Array<String> const&);

  template Bool anyEQ(Array<Bool> const &, Bool const &);
  template Bool anyEQ(Array<Int> const &, Int const &);
  template Bool anyEQ(Array<uInt> const &, uInt const &);
  template Bool anyEQ(Array<Short> const &, Short const &);
  template Bool anyEQ(Array<uShort> const &, uShort const &);
  template Bool anyEQ(Array<Long> const &, Long const &);
  template Bool anyEQ(Array<uLong> const &, uLong const &);
  template Bool anyEQ(Array<float> const &, float const &);
  template Bool anyEQ(Array<double> const &, double const &);
  template Bool anyEQ(Array<Complex> const &, Complex const &);
  template Bool anyEQ(Array<DComplex> const &, DComplex const &);
  template Bool anyEQ(Array<String> const &, String const &);

  template Bool anyEQ(Array<bool> const &, Array<bool> const &);
  template Bool anyEQ(Array<Int> const &, Array<Int> const &);
  template Bool anyEQ(Array<uInt> const &, Array<uInt> const &);
  template Bool anyEQ(Array<Short> const &, Array<Short> const &);
  template Bool anyEQ(Array<uShort> const &, Array<uShort> const &);
  template Bool anyEQ(Array<Long> const &, Array<Long> const &);
  template Bool anyEQ(Array<uLong> const &, Array<uLong> const &);
  template Bool anyEQ(Array<Float> const &, Array<Float> const &);
  template Bool anyEQ(Array<Double> const &, Array<Double> const &);
  template Bool anyEQ(Array<Complex> const &, Array<Complex> const &);
  template Bool anyEQ(Array<DComplex> const &, Array<DComplex> const &);
  template Bool anyEQ(Array<String> const &, Array<String> const &);

  // anyGE

  template bool anyGE(Bool const&, Array<Bool> const&);
  template bool anyGE(Int const&, Array<Int> const&);
  template bool anyGE(uInt const&, Array<uInt> const&);
  template bool anyGE(Short const&, Array<Short> const&);
  template bool anyGE(uShort const&, Array<uShort> const&);
  template bool anyGE(Long const&, Array<Long> const&);
  template bool anyGE(uLong const&, Array<uLong> const&);
  template bool anyGE(Float const&, Array<Float> const&);
  template bool anyGE(Double const&, Array<Double> const&);
  template bool anyGE(Complex const&, Array<Complex> const&);
  template bool anyGE(DComplex const&, Array<DComplex> const&);
  template bool anyGE(String const&, Array<String> const&);

  template Bool anyGE(Array<Bool> const &, Bool const &);
  template Bool anyGE(Array<Int> const &, Int const &);
  template Bool anyGE(Array<uInt> const &, uInt const &);
  template Bool anyGE(Array<Short> const &, Short const &);
  template Bool anyGE(Array<uShort> const &, uShort const &);
  template Bool anyGE(Array<Long> const &, Long const &);
  template Bool anyGE(Array<uLong> const &, uLong const &);
  template Bool anyGE(Array<float> const &, float const &);
  template Bool anyGE(Array<double> const &, double const &);
  template Bool anyGE(Array<Complex> const &, Complex const &);
  template Bool anyGE(Array<DComplex> const &, DComplex const &);
  template Bool anyGE(Array<String> const &, String const &);

  template Bool anyGE(Array<bool> const &, Array<bool> const &);
  template Bool anyGE(Array<Int> const &, Array<Int> const &);
  template Bool anyGE(Array<uInt> const &, Array<uInt> const &);
  template Bool anyGE(Array<Short> const &, Array<Short> const &);
  template Bool anyGE(Array<uShort> const &, Array<uShort> const &);
  template Bool anyGE(Array<Long> const &, Array<Long> const &);
  template Bool anyGE(Array<uLong> const &, Array<uLong> const &);
  template Bool anyGE(Array<Float> const &, Array<Float> const &);
  template Bool anyGE(Array<Double> const &, Array<Double> const &);
  template Bool anyGE(Array<Complex> const &, Array<Complex> const &);
  template Bool anyGE(Array<DComplex> const &, Array<DComplex> const &);
  template Bool anyGE(Array<String> const &, Array<String> const &);

  // anyGT

  template bool anyGT(Bool const&, Array<Bool> const&);
  template bool anyGT(Int const&, Array<Int> const&);
  template bool anyGT(uInt const&, Array<uInt> const&);
  template bool anyGT(Short const&, Array<Short> const&);
  template bool anyGT(uShort const&, Array<uShort> const&);
  template bool anyGT(Long const&, Array<Long> const&);
  template bool anyGT(uLong const&, Array<uLong> const&);
  template bool anyGT(Float const&, Array<Float> const&);
  template bool anyGT(Double const&, Array<Double> const&);
  template bool anyGT(Complex const&, Array<Complex> const&);
  template bool anyGT(DComplex const&, Array<DComplex> const&);
  template bool anyGT(String const&, Array<String> const&);

  template Bool anyGT(Array<Bool> const &, Bool const &);
  template Bool anyGT(Array<Int> const &, Int const &);
  template Bool anyGT(Array<uInt> const &, uInt const &);
  template Bool anyGT(Array<Short> const &, Short const &);
  template Bool anyGT(Array<uShort> const &, uShort const &);
  template Bool anyGT(Array<Long> const &, Long const &);
  template Bool anyGT(Array<uLong> const &, uLong const &);
  template Bool anyGT(Array<float> const &, float const &);
  template Bool anyGT(Array<double> const &, double const &);
  template Bool anyGT(Array<Complex> const &, Complex const &);
  template Bool anyGT(Array<DComplex> const &, DComplex const &);
  template Bool anyGT(Array<String> const &, String const &);

  template Bool anyGT(Array<bool> const &, Array<bool> const &);
  template Bool anyGT(Array<Int> const &, Array<Int> const &);
  template Bool anyGT(Array<uInt> const &, Array<uInt> const &);
  template Bool anyGT(Array<Short> const &, Array<Short> const &);
  template Bool anyGT(Array<uShort> const &, Array<uShort> const &);
  template Bool anyGT(Array<Long> const &, Array<Long> const &);
  template Bool anyGT(Array<uLong> const &, Array<uLong> const &);
  template Bool anyGT(Array<Float> const &, Array<Float> const &);
  template Bool anyGT(Array<Double> const &, Array<Double> const &);
  template Bool anyGT(Array<Complex> const &, Array<Complex> const &);
  template Bool anyGT(Array<DComplex> const &, Array<DComplex> const &);
  template Bool anyGT(Array<String> const &, Array<String> const &);

  // anyLE

  template bool anyLE(Bool const&, Array<Bool> const&);
  template bool anyLE(Int const&, Array<Int> const&);
  template bool anyLE(uInt const&, Array<uInt> const&);
  template bool anyLE(Short const&, Array<Short> const&);
  template bool anyLE(uShort const&, Array<uShort> const&);
  template bool anyLE(Long const&, Array<Long> const&);
  template bool anyLE(uLong const&, Array<uLong> const&);
  template bool anyLE(Float const&, Array<Float> const&);
  template bool anyLE(Double const&, Array<Double> const&);
  template bool anyLE(Complex const&, Array<Complex> const&);
  template bool anyLE(DComplex const&, Array<DComplex> const&);
  template bool anyLE(String const&, Array<String> const&);

  template Bool anyLE(Array<Bool> const &, Bool const &);
  template Bool anyLE(Array<Int> const &, Int const &);
  template Bool anyLE(Array<uInt> const &, uInt const &);
  template Bool anyLE(Array<Short> const &, Short const &);
  template Bool anyLE(Array<uShort> const &, uShort const &);
  template Bool anyLE(Array<Long> const &, Long const &);
  template Bool anyLE(Array<uLong> const &, uLong const &);
  template Bool anyLE(Array<float> const &, float const &);
  template Bool anyLE(Array<double> const &, double const &);
  template Bool anyLE(Array<Complex> const &, Complex const &);
  template Bool anyLE(Array<DComplex> const &, DComplex const &);
  template Bool anyLE(Array<String> const &, String const &);

  template Bool anyLE(Array<bool> const &, Array<bool> const &);
  template Bool anyLE(Array<Int> const &, Array<Int> const &);
  template Bool anyLE(Array<uInt> const &, Array<uInt> const &);
  template Bool anyLE(Array<Short> const &, Array<Short> const &);
  template Bool anyLE(Array<uShort> const &, Array<uShort> const &);
  template Bool anyLE(Array<Long> const &, Array<Long> const &);
  template Bool anyLE(Array<uLong> const &, Array<uLong> const &);
  template Bool anyLE(Array<Float> const &, Array<Float> const &);
  template Bool anyLE(Array<Double> const &, Array<Double> const &);
  template Bool anyLE(Array<Complex> const &, Array<Complex> const &);
  template Bool anyLE(Array<DComplex> const &, Array<DComplex> const &);
  template Bool anyLE(Array<String> const &, Array<String> const &);

  // anyLT

  template bool anyLT(Bool const&, Array<Bool> const&);
  template bool anyLT(Int const&, Array<Int> const&);
  template bool anyLT(uInt const&, Array<uInt> const&);
  template bool anyLT(Short const&, Array<Short> const&);
  template bool anyLT(uShort const&, Array<uShort> const&);
  template bool anyLT(Long const&, Array<Long> const&);
  template bool anyLT(uLong const&, Array<uLong> const&);
  template bool anyLT(Float const&, Array<Float> const&);
  template bool anyLT(Double const&, Array<Double> const&);
  template bool anyLT(Complex const&, Array<Complex> const&);
  template bool anyLT(DComplex const&, Array<DComplex> const&);
  template bool anyLT(String const&, Array<String> const&);

  template Bool anyLT(Array<Bool> const &, Bool const &);
  template Bool anyLT(Array<Int> const &, Int const &);
  template Bool anyLT(Array<uInt> const &, uInt const &);
  template Bool anyLT(Array<Short> const &, Short const &);
  template Bool anyLT(Array<uShort> const &, uShort const &);
  template Bool anyLT(Array<Long> const &, Long const &);
  template Bool anyLT(Array<uLong> const &, uLong const &);
  template Bool anyLT(Array<float> const &, float const &);
  template Bool anyLT(Array<double> const &, double const &);
  template Bool anyLT(Array<Complex> const &, Complex const &);
  template Bool anyLT(Array<DComplex> const &, DComplex const &);
  template Bool anyLT(Array<String> const &, String const &);

  template Bool anyLT(Array<bool> const &, Array<bool> const &);
  template Bool anyLT(Array<Int> const &, Array<Int> const &);
  template Bool anyLT(Array<uInt> const &, Array<uInt> const &);
  template Bool anyLT(Array<Short> const &, Array<Short> const &);
  template Bool anyLT(Array<uShort> const &, Array<uShort> const &);
  template Bool anyLT(Array<Long> const &, Array<Long> const &);
  template Bool anyLT(Array<uLong> const &, Array<uLong> const &);
  template Bool anyLT(Array<Float> const &, Array<Float> const &);
  template Bool anyLT(Array<Double> const &, Array<Double> const &);
  template Bool anyLT(Array<Complex> const &, Array<Complex> const &);
  template Bool anyLT(Array<DComplex> const &, Array<DComplex> const &);
  template Bool anyLT(Array<String> const &, Array<String> const &);

  // anyNE

  template bool anyNE(Bool const&, Array<Bool> const&);
  template bool anyNE(Int const&, Array<Int> const&);
  template bool anyNE(uInt const&, Array<uInt> const&);
  template bool anyNE(Short const&, Array<Short> const&);
  template bool anyNE(uShort const&, Array<uShort> const&);
  template bool anyNE(Long const&, Array<Long> const&);
  template bool anyNE(uLong const&, Array<uLong> const&);
  template bool anyNE(Float const&, Array<Float> const&);
  template bool anyNE(Double const&, Array<Double> const&);
  template bool anyNE(Complex const&, Array<Complex> const&);
  template bool anyNE(DComplex const&, Array<DComplex> const&);
  template bool anyNE(String const&, Array<String> const&);

  template Bool anyNE(Array<Bool> const &, Bool const &);
  template Bool anyNE(Array<Int> const &, Int const &);
  template Bool anyNE(Array<uInt> const &, uInt const &);
  template Bool anyNE(Array<Short> const &, Short const &);
  template Bool anyNE(Array<uShort> const &, uShort const &);
  template Bool anyNE(Array<Long> const &, Long const &);
  template Bool anyNE(Array<uLong> const &, uLong const &);
  template Bool anyNE(Array<float> const &, float const &);
  template Bool anyNE(Array<double> const &, double const &);
  template Bool anyNE(Array<Complex> const &, Complex const &);
  template Bool anyNE(Array<DComplex> const &, DComplex const &);
  template Bool anyNE(Array<String> const &, String const &);

  template Bool anyNE(Array<bool> const &, Array<bool> const &);
  template Bool anyNE(Array<Int> const &, Array<Int> const &);
  template Bool anyNE(Array<uInt> const &, Array<uInt> const &);
  template Bool anyNE(Array<Short> const &, Array<Short> const &);
  template Bool anyNE(Array<uShort> const &, Array<uShort> const &);
  template Bool anyNE(Array<Long> const &, Array<Long> const &);
  template Bool anyNE(Array<uLong> const &, Array<uLong> const &);
  template Bool anyNE(Array<Float> const &, Array<Float> const &);
  template Bool anyNE(Array<Double> const &, Array<Double> const &);
  template Bool anyNE(Array<Complex> const &, Array<Complex> const &);
  template Bool anyNE(Array<DComplex> const &, Array<DComplex> const &);
  template Bool anyNE(Array<String> const &, Array<String> const &);

  // allNear

  template bool allNear(Array<Float> const&, Array<Float> const&, double);
  template bool allNear(Array<Double> const&, Array<Double> const&, double);
  template bool allNear(Array<Complex> const&, Array<Complex> const&, double);
  template bool allNear(Array<DComplex> const&, Array<DComplex> const&, double);

  // allNear

  template bool allNear(Float const&, Array<Float> const&, double);
  template bool allNear(Double const&, Array<Double> const&, double);
  template bool allNear(Complex const&, Array<Complex> const&, double);
  template bool allNear(DComplex const&, Array<DComplex> const&, double);

  // allNearAbs

  template Bool allNearAbs(Float const &, Array<Float> const &, Double);
  template Bool allNearAbs(Double const &, Array<Double> const &, Double);
  template Bool allNearAbs(Complex const &, Array<Complex> const &, Double);
  template Bool allNearAbs(DComplex const &, Array<DComplex> const &, Double);

  template Bool allNearAbs(Array<Float> const &, Float const &, Double);
  template Bool allNearAbs(Array<Double> const &, Double const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Complex const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, DComplex const &, Double);

  template Bool allNearAbs(Array<Float> const &, Array<Float> const &, Double);
  template Bool allNearAbs(Array<Double> const &, Array<Double> const &, Double);
  template Bool allNearAbs(Array<Complex> const &, Array<Complex> const &, Double);
  template Bool allNearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);

  // anyNear

  template bool anyNear(Float const&, Array<Float> const&, Double);
  template bool anyNear(Double const&, Array<Double> const&, Double);
  template bool anyNear(Complex const&, Array<Complex> const&, Double);
  template bool anyNear(DComplex const&, Array<DComplex> const&, Double);

  template bool anyNear(Array<Float> const&, Float const&, Double);
  template bool anyNear(Array<Double> const&, Double const&, Double);
  template bool anyNear(Array<Complex> const&, Complex const&, Double);
  template bool anyNear(Array<DComplex> const&, DComplex const&, Double);

  template bool anyNear(Array<Float> const&, Array<Float> const&, Double);
  template bool anyNear(Array<Double> const&, Array<Double> const&, Double);
  template bool anyNear(Array<Complex> const&, Array<Complex> const&, Double);
  template bool anyNear(Array<DComplex> const&, Array<DComplex> const&, Double);

  // anyNearAbs

  template bool anyNearAbs(Float const&, Array<Float> const&, Double);
  template bool anyNearAbs(Double const&, Array<Double> const&, Double);
  template bool anyNearAbs(Complex const&, Array<Complex> const&, Double);
  template bool anyNearAbs(DComplex const&, Array<DComplex> const&, Double);

  template bool anyNearAbs(Array<Float> const&, Float const&, Double);
  template bool anyNearAbs(Array<Double> const&, Double const&, Double);
  template bool anyNearAbs(Array<Complex> const&, Complex const&, Double);
  template bool anyNearAbs(Array<DComplex> const&, DComplex const&, Double);

  template bool anyNearAbs(Array<Float> const&, Array<Float> const&, Double);
  template bool anyNearAbs(Array<Double> const&, Array<Double> const&, Double);
  template bool anyNearAbs(Array<Complex> const&, Array<Complex> const&, Double);
  template bool anyNearAbs(Array<DComplex> const&, Array<DComplex> const&, Double);

  // ---------------------------------------------

  template Array<Bool> operator<(Int const &, Array<Int> const &);
  template Array<Bool> operator<(uInt const &, Array<uInt> const &);
  template Array<Bool> operator<(Short const &, Array<Short> const &);
  template Array<Bool> operator<(Long const &, Array<Long> const &);
  template Array<Bool> operator<(Float const &, Array<Float> const &);
  template Array<Bool> operator<(Double const &, Array<Double> const &);
  template Array<Bool> operator<(Complex const &, Array<Complex> const &);
  template Array<Bool> operator<(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator<(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator<(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator<(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator<(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator<(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator<(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator<(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator<(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator<=(Int const &, Array<Int> const &);
  template Array<Bool> operator<=(uInt const &, Array<uInt> const &);
  template Array<Bool> operator<=(Short const &, Array<Short> const &);
  template Array<Bool> operator<=(Long const &, Array<Long> const &);
  template Array<Bool> operator<=(Float const &, Array<Float> const &);
  template Array<Bool> operator<=(Double const &, Array<Double> const &);
  template Array<Bool> operator<=(Complex const &, Array<Complex> const &);
  template Array<Bool> operator<=(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator<=(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator<=(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator<=(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator<=(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator<=(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator<=(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator<=(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator<=(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator==(Int const &, Array<Int> const &);
  template Array<Bool> operator==(uInt const &, Array<uInt> const &);
  template Array<Bool> operator==(Short const &, Array<Short> const &);
  template Array<Bool> operator==(Long const &, Array<Long> const &);
  template Array<Bool> operator==(Float const &, Array<Float> const &);
  template Array<Bool> operator==(Double const &, Array<Double> const &);
  template Array<Bool> operator==(Complex const &, Array<Complex> const &);
  template Array<Bool> operator==(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator==(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator==(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator==(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator==(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator==(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator==(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator==(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator==(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator>(Int const &, Array<Int> const &);
  template Array<Bool> operator>(uInt const &, Array<uInt> const &);
  template Array<Bool> operator>(Short const &, Array<Short> const &);
  template Array<Bool> operator>(Long const &, Array<Long> const &);
  template Array<Bool> operator>(Float const &, Array<Float> const &);
  template Array<Bool> operator>(Double const &, Array<Double> const &);
  template Array<Bool> operator>(Complex const &, Array<Complex> const &);
  template Array<Bool> operator>(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator>(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator>(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator>(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator>(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator>(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator>(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator>(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator>(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator>=(Int const &, Array<Int> const &);
  template Array<Bool> operator>=(uInt const &, Array<uInt> const &);
  template Array<Bool> operator>=(Short const &, Array<Short> const &);
  template Array<Bool> operator>=(Long const &, Array<Long> const &);
  template Array<Bool> operator>=(Float const &, Array<Float> const &);
  template Array<Bool> operator>=(Double const &, Array<Double> const &);
  template Array<Bool> operator>=(Complex const &, Array<Complex> const &);
  template Array<Bool> operator>=(DComplex const &, Array<DComplex> const &);

  template Array<Bool> operator>=(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator>=(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator>=(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator>=(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator>=(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator>=(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator>=(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator>=(Array<DComplex> const &, Array<DComplex> const &);
  
  template Array<Bool> operator!=(Array<Int> const &, Array<Int> const &);
  template Array<Bool> operator!=(Array<uInt> const &, Array<uInt> const &);
  template Array<Bool> operator!=(Array<Short> const &, Array<Short> const &);
  template Array<Bool> operator!=(Array<Long> const &, Array<Long> const &);
  template Array<Bool> operator!=(Array<Float> const &, Array<Float> const &);
  template Array<Bool> operator!=(Array<Double> const &, Array<Double> const &);
  template Array<Bool> operator!=(Array<Complex> const &, Array<Complex> const &);
  template Array<Bool> operator!=(Array<DComplex> const &, Array<DComplex> const &);

  template Array<Bool> operator!=(Int const &, Array<Int> const &);
  template Array<Bool> operator!=(uInt const &, Array<uInt> const &);
  template Array<Bool> operator!=(Short const &, Array<Short> const &);
  template Array<Bool> operator!=(Long const &, Array<Long> const &);
  template Array<Bool> operator!=(Float const &, Array<Float> const &);
  template Array<Bool> operator!=(Double const &, Array<Double> const &);
  template Array<Bool> operator!=(Complex const &, Array<Complex> const &);
  template Array<Bool> operator!=(DComplex const &, Array<DComplex> const &);
  
  template Array<Bool> operator!=(Array<Complex> const &, Complex const &);
  template Array<Bool> operator<(Array<Complex> const &, Complex const &);
  template Array<Bool> operator<=(Array<Complex> const &, Complex const &);
  template Array<Bool> operator==(Array<Complex> const &, Complex const &);
  template Array<Bool> operator>(Array<Complex> const &, Complex const &);
  template Array<Bool> operator>=(Array<Complex> const &, Complex const &);
  template Bool allNear(Array<Complex> const &, Complex const &, Double);
  template LogicalArray operator!=(Array<DComplex> const &, DComplex const &);
  template LogicalArray operator==(Array<DComplex> const &, DComplex const &);
  template LogicalArray operator>(Array<DComplex> const &, DComplex const &);
  template LogicalArray operator>=(Array<DComplex> const &, DComplex const &);
  template LogicalArray near(Array<DComplex> const &, Array<DComplex> const &, Double);
  template LogicalArray near(Array<DComplex> const &, DComplex const &, Double);
  template LogicalArray near(DComplex const &, Array<DComplex> const &, Double);
  template LogicalArray nearAbs(Array<DComplex> const &, Array<DComplex> const &, Double);
  template LogicalArray nearAbs(Array<DComplex> const &, DComplex const &, Double);
  template LogicalArray nearAbs(DComplex const &, Array<DComplex> const &, Double);
  template LogicalArray operator!=(Array<String> const &, Array<String> const &);
  template LogicalArray operator!=(Array<String> const &, String const &);
  template LogicalArray operator!=(String const &, Array<String> const &);
  template LogicalArray operator==(Array<String> const &, Array<String> const &);
  template LogicalArray operator==(Array<String> const &, String const &);
  template LogicalArray operator==(String const &, Array<String> const &);
  template LogicalArray operator>(Array<String> const &, Array<String> const &);
  template LogicalArray operator>(Array<String> const &, String const &);
  template LogicalArray operator>(String const &, Array<String> const &);
  template LogicalArray operator>=(Array<String> const &, Array<String> const &);
  template LogicalArray operator>=(Array<String> const &, String const &);
  template LogicalArray operator>=(String const &, Array<String> const &);
  template Bool anyEQ(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator==(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator!=(Array<MVTime> const &, Array<MVTime> const &);
  template LogicalArray operator!=(Array<MVTime> const &, MVTime const &);
  template LogicalArray operator!=(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator>(Array<MVTime> const &, Array<MVTime> const &);
  template LogicalArray operator>(Array<MVTime> const &, MVTime const &);
  template LogicalArray operator>(MVTime const &, Array<MVTime> const &);
  template LogicalArray operator>=(Array<MVTime> const &, Array<MVTime> const &);
  template LogicalArray operator>=(Array<MVTime> const &, MVTime const &);
  template LogicalArray operator>=(MVTime const &, Array<MVTime> const &);
  template Bool allNear(Array<Double> const &, Double const &, Double);
  template LogicalArray operator!=(Array<Double> const &, Double const &);
  template LogicalArray operator==(Array<Double> const &, Double const &);
  template LogicalArray operator>(Array<Double> const &, Double const &);
  template LogicalArray operator>=(Array<Double> const &, Double const &);
  template LogicalArray operator<=(Array<Double> const &, Double const &);
  template LogicalArray near(Array<Double> const &, Array<Double> const &, Double);
  template LogicalArray near(Array<Double> const &, Double const &, Double);
  template LogicalArray near(Double const &, Array<Double> const &, Double);
  template LogicalArray nearAbs(Array<Double> const &, Array<Double> const &, Double);
  template LogicalArray nearAbs(Array<Double> const &, Double const &, Double);
  template LogicalArray nearAbs(Double const &, Array<Double> const &, Double);
  template Array<Bool> operator!=(Array<Int> const &, Int const &);
  template Array<Bool> operator==(Array<Int> const &, Int const &);
  template Array<Bool> operator>(Array<Int> const &, Int const &);
  template Array<Bool> operator<(Array<Int> const &, Int const &);
  template Array<Bool> operator>=(Array<Int> const &, Int const &);
  template Array<Bool> operator<=(Array<Int> const &, Int const &);
  template Array<Bool> operator==(Array<Short> const &, Short const &);
  template Array<Bool> operator>(Array<Short> const &, Short const &);
  template Bool allEQ(Array<uChar> const &, Array<uChar> const &);
  template Bool allEQ(Array<uChar> const &, uChar const &);
  template Array<Bool> operator==(Array<uChar> const &, uChar const &);
  template Array<Bool> operator>(Array<uChar> const &, uChar const &);
  template Array<Bool> operator!=(Array<uInt> const &, uInt const &);
  template Array<Bool> operator==(Array<uInt> const &, uInt const &);
  template Array<Bool> operator>(Array<uInt> const &, uInt const &);
  template Array<Bool> operator<(Array<uInt> const &, uInt const &);
  template Array<Bool> operator||(Array<uInt> const &, Array<uInt> const &);
  template Bool allNear(Array<Float> const &, Float const &, Double);
  template LogicalArray operator!=(Array<Float> const &, Float const &);
  template LogicalArray operator<(Array<Float> const &, Float const &);
  template LogicalArray operator<=(Array<Float> const &, Float const &);
  template LogicalArray operator==(Array<Float> const &, Float const &);
  template LogicalArray operator>(Array<Float> const &, Float const &);
  template LogicalArray operator>=(Array<Float> const &, Float const &);
  template Bool allAND(Array<Bool> const &, Bool const &);
  template Bool allOR(Array<Bool> const &, Array<Bool> const &);
  template uInt ntrue(Array<Bool> const &);
  template uInt nfalse(Array<Bool> const &);
  template LogicalArray operator!(Array<Bool> const &);
  template LogicalArray operator!=(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator!=(Array<Bool> const &, Bool const &);
  template LogicalArray operator!=(Bool const &, Array<Bool> const &);
  template LogicalArray operator&&(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator&&(Array<Bool> const &, Bool const &);
  template LogicalArray operator&&(Bool const &, Array<Bool> const &);
  template LogicalArray operator==(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator==(Array<Bool> const &, Bool const &);
  template LogicalArray operator==(Bool const &, Array<Bool> const &);
  template LogicalArray operator||(Array<Bool> const &, Array<Bool> const &);
  template LogicalArray operator||(Array<Bool> const &, Bool const &);
  template LogicalArray operator||(Bool const &, Array<Bool> const &);
  template Array<uInt> partialNTrue(Array<Bool> const &, IPosition const &);
  template Array<uInt> partialNFalse(Array<Bool> const &, IPosition const &);
  
  template Array<Float> pow(Float const &, Array<Float> const &);
  template Array<Double> pow(Double const &, Array<Double> const &);
  template Array<Complex> pow(Complex const &, Array<Complex> const &);
  template Array<DComplex> pow(DComplex const &, Array<DComplex> const &);

  template Array<Float> pow(Array<Float> const &, Array<Float> const &);
  template Array<Double> pow(Array<Double> const &, Array<Double> const &);
  template Array<Complex> pow(Array<Complex> const &, Array<Complex> const &);
  template Array<DComplex> pow(Array<DComplex> const &, Array<DComplex> const &);

  template Array<Float> pow(Array<Float> const &, Double const &);
  template Array<Double> pow(Array<Double> const &, Double const &);
  template Array<Complex> pow(Array<Complex> const &, Double const &);
  template Array<DComplex> pow(Array<DComplex> const &, Double const &);

  template void convertArray(Array<Double> &, Array<MVTime> const &);
  template Array<Complex> abs(Array<Complex> const &);
  template Array<Complex> cos(Array<Complex> const &);
  template Array<Complex> cosh(Array<Complex> const &);
  template Array<Complex> exp(Array<Complex> const &);
  template Array<Complex> log(Array<Complex> const &);
  template Array<Complex> log10(Array<Complex> const &);
  template Array<Complex> operator*(Array<Complex> const &, Complex const &);
  template Array<Complex> operator+(Array<Complex> const &, Array<Complex> const &);
  template Array<Complex> operator-(Array<Complex> const &);
  template Array<Complex> operator-(Complex const &, Array<Complex> const &);
  template Array<Complex> operator-(Array<Complex> const &, Array<Complex> const &);
  template Array<Complex> operator/(Array<Complex> const &, Complex const &);
  template Array<Complex> operator/(Complex const &, Array<Complex> const &);
  template Array<Complex> sin(Array<Complex> const &);
  template Array<Complex> sinh(Array<Complex> const &);
  template Array<Complex> sqrt(Array<Complex> const &);
  template Complex max(Array<Complex> const &);
  template Complex min(Array<Complex> const &);
  template void minMax(Complex &, Complex &, Array<Complex> const &);
  template void operator*=(Array<Complex> &, Array<Complex> const &);
  template void operator*=(Array<Complex> &, Complex const &);
  template void operator+=(Array<Complex> &, Array<Complex> const &);
  template void operator+=(Array<Complex> &, Complex const &);
  template void operator-=(Array<Complex> &, Array<Complex> const &);
  template void operator-=(Array<Complex> &, Complex const &);
  template void operator/=(Array<Complex> &, Array<Complex> const &);
  template void operator/=(Array<Complex> &, Complex const &);
  template Array<DComplex> abs(Array<DComplex> const &);
  template Array<DComplex> cos(Array<DComplex> const &);
  template Array<DComplex> cosh(Array<DComplex> const &);
  template Array<DComplex> exp(Array<DComplex> const &);
  template Array<DComplex> log(Array<DComplex> const &);
  template Array<DComplex> log10(Array<DComplex> const &);
  template Array<DComplex> max(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> max(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> min(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> min(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator*(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator*(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator*(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> operator+(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator+(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator+(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> operator-(Array<DComplex> const &);
  template Array<DComplex> operator-(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator-(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator-(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> operator/(Array<DComplex> const &, Array<DComplex> const &);
  template Array<DComplex> operator/(Array<DComplex> const &, DComplex const &);
  template Array<DComplex> operator/(DComplex const &, Array<DComplex> const &);
  template Array<DComplex> sin(Array<DComplex> const &);
  template Array<DComplex> sinh(Array<DComplex> const &);
  template Array<DComplex> sqrt(Array<DComplex> const &);
  template DComplex max(Array<DComplex> const &);
  template DComplex min(Array<DComplex> const &);
  template DComplex product(Array<DComplex> const &);
  template DComplex sum(Array<DComplex> const &);
  template Complex sum(Array<Complex> const &);
  template void max(Array<DComplex> &, Array<DComplex> const &, Array<DComplex> const &);
  template void max(Array<DComplex> &, Array<DComplex> const &, DComplex const &);
  template void min(Array<DComplex> &, Array<DComplex> const &, Array<DComplex> const &);
  template void min(Array<DComplex> &, Array<DComplex> const &, DComplex const &);
  template void minMax(DComplex &, DComplex &, Array<DComplex> const &);
  template void operator*=(Array<DComplex> &, Array<DComplex> const &);
  template void operator*=(Array<DComplex> &, DComplex const &);
  template void operator+=(Array<DComplex> &, Array<DComplex> const &);
  template void operator+=(Array<DComplex> &, DComplex const &);
  template void operator-=(Array<DComplex> &, Array<DComplex> const &);
  template void operator-=(Array<DComplex> &, DComplex const &);
  template void operator/=(Array<DComplex> &, Array<DComplex> const &);
  template void operator/=(Array<DComplex> &, DComplex const &);
  template DComplex mean(Array<DComplex> const &);
  template DComplex median(Array<DComplex> const &, Bool);
  template DComplex median(Array<DComplex> const &, Bool, Bool, Bool);
  template Float product(Array<Float> const &); 
  template Array<Bool> operator*(Array<Bool> const &, Array<Bool> const &);
  template void convertArray(Array<Complex> &, Array<DComplex> const &);
  template void convertArray(Array<Complex> &, Array<Double> const &);
  template void convertArray(Array<Complex> &, Array<Float> const &);
  template void convertArray(Array<Complex> &, Array<Int> const &);
  template void convertArray(Array<Complex> &, Array<Short> const &);
  template void convertArray(Array<Complex> &, Array<uChar> const &);
  template void convertArray(Array<Complex> &, Array<uInt> const &);
  template void convertArray(Array<DComplex> &, Array<Complex> const &);
  template void convertArray(Array<DComplex> &, Array<DComplex> const &);
  template void convertArray(Array<DComplex> &, Array<Double> const &);
  template void convertArray(Array<DComplex> &, Array<Float> const &);
  template void convertArray(Array<DComplex> &, Array<Int> const &);
  template void convertArray(Array<DComplex> &, Array<Short> const &);
  template void convertArray(Array<DComplex> &, Array<uChar> const &);
  template void convertArray(Array<DComplex> &, Array<uInt> const &);
  template void convertArray(Array<uInt> &, Array<Bool> const &);
  template Array<String> operator+(String const &, Array<String> const &);
  template void operator+=(Array<String> &, Array<String> const &);
  template void operator+=(Array<String> &, String const &);
  template Array<Double> abs(Array<Double> const &);
  template Array<Double> acos(Array<Double> const &);
  template Array<Double> asin(Array<Double> const &);
  template Array<Double> atan(Array<Double> const &);
  template Array<Double> atan2(Array<Double> const &, Array<Double> const &);
  template Array<Double> ceil(Array<Double> const &);
  template Array<Double> cos(Array<Double> const &);
  template Array<Double> cosh(Array<Double> const &);
  template Array<Double> exp(Array<Double> const &);
  template Array<Double> fabs(Array<Double> const &);
  template Array<Double> floor(Array<Double> const &);
  template Array<Double> fmod(Array<Double> const &, Array<Double> const &);
  template Array<Double> fmod(Array<Double> const &, Double const &);
  template Array<Double> fmod(Double const &, Array<Double> const &);
  template Array<Double> log(Array<Double> const &);
  template Array<Double> log10(Array<Double> const &);
  template Array<Double> max(Array<Double> const &, Array<Double> const &);
  template Array<Double> max(Array<Double> const &, Double const &);
  template Array<Double> min(Array<Double> const &, Array<Double> const &);
  template Array<Double> min(Array<Double> const &, Double const &);
  template Array<Double> operator*(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator*(Array<Double> const &, Double const &);
  template Array<Double> operator*(Double const &, Array<Double> const &);
  template Array<Double> operator+(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator+(Array<Double> const &, Double const &);
  template Array<Double> operator+(Double const &, Array<Double> const &);
  template Array<Double> operator-(Array<Double> const &);
  template Array<Double> operator-(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator-(Array<Double> const &, Double const &);
  template Array<Double> operator-(Double const &, Array<Double> const &);
  template Array<Double> operator/(Array<Double> const &, Array<Double> const &);
  template Array<Double> operator/(Array<Double> const &, Double const &);
  template Array<Double> operator/(Double const &, Array<Double> const &);
  template Array<Double> sin(Array<Double> const &);
  template Array<Double> sinh(Array<Double> const &);
  template Array<Double> sqrt(Array<Double> const &);
  template Array<Double> tan(Array<Double> const &);
  template Array<Double> tanh(Array<Double> const &);
  template Double avdev(Array<Double> const &);
  template Double avdev(Array<Double> const &, Double);
  template Double fractile(Array<Double> const &, Float, Bool, Bool);
  template Double max(Array<Double> const &);
  template Double mean(Array<Double> const &);
  template Double median(Array<Double> const &, Bool);
  template Double median(Array<Double> const &, Bool, Bool, Bool);
  template Double min(Array<Double> const &);
  template Double product(Array<Double> const &);
  template Double stddev(Array<Double> const &);
  template Double sum(Array<Double> const &);
  template Double variance(Array<Double> const &);
  template Double variance(Array<Double> const &, Double);
  template void indgen(Array<Double> &);
  template void indgen(Array<Double> &, Double);
  template void indgen(Array<Double> &, Double, Double);
  template void max(Array<Double> &, Array<Double> const &, Array<Double> const &);
  template void max(Array<Double> &, Array<Double> const &, Double const &);
  template void min(Array<Double> &, Array<Double> const &, Array<Double> const &);
  template void min(Array<Double> &, Array<Double> const &, Double const &);
  template void minMax(Double &, Double &, Array<Double> const &);
  template void minMax(Double &, Double &, IPosition &, IPosition &, Array<Double> const &);
  template void minMax(Double &, Double &, IPosition &, IPosition &, Array<Double> const &, Array<Bool> const &);
  template void operator*=(Array<Double> &, Array<Double> const &);
  template void operator*=(Array<Double> &, Double const &);
  template void operator+=(Array<Double> &, Array<Double> const &);
  template void operator+=(Array<Double> &, Double const &);
  template void operator-=(Array<Double> &, Array<Double> const &);
  template void operator-=(Array<Double> &, Double const &);
  template void operator/=(Array<Double> &, Array<Double> const &);
  template void operator/=(Array<Double> &, Double const &);
  template Array<Float> abs(Array<Float> const &);
  template Array<Float> acos(Array<Float> const &);
  template Array<Float> asin(Array<Float> const &);
  template Array<Float> atan(Array<Float> const &);
  template Array<Float> atan2(Array<Float> const &, Array<Float> const &);
  template Array<Float> ceil(Array<Float> const &);
  template Array<Float> cos(Array<Float> const &);
  template Array<Float> cosh(Array<Float> const &);
  template Array<Float> exp(Array<Float> const &);
  template Array<Float> floor(Array<Float> const &);
  template Array<Float> fmod(Array<Float> const &, Array<Float> const &);
  template Array<Float> log(Array<Float> const &);
  template Array<Float> log10(Array<Float> const &);
  template Array<Float> max(Array<Float> const &, Array<Float> const &);
  template Array<Float> min(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator*(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator*(Array<Float> const &, Float const &);
  template Array<Float> operator*(Float const &, Array<Float> const &);
  template Array<Float> operator+(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator+(Array<Float> const &, Float const &);
  template Array<Float> operator-(Array<Float> const &);
  template Array<Float> operator-(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator-(Array<Float> const &, Float const &);
  template Array<Float> operator-(Float const &, Array<Float> const &);
  template Array<Float> operator/(Array<Float> const &, Array<Float> const &);
  template Array<Float> operator/(Array<Float> const &, Float const &);
  template Array<Float> operator/(Float const &, Array<Float> const &);
  template Array<Float> sin(Array<Float> const &);
  template Array<Float> sinh(Array<Float> const &);
  template Array<Float> sqrt(Array<Float> const &);
  template Array<Float> tan(Array<Float> const &);
  template Array<Float> tanh(Array<Float> const &);
  template Float fractile(Array<Float> const &, Float, Bool, Bool);
  template Float max(Array<Float> const &);
  template Float mean(Array<Float> const &);
  template Float median(Array<Float> const &, Bool);
  template Float median(Array<Float> const &, Bool, Bool, Bool);
  template Float min(Array<Float> const &);
  template Float stddev(Array<Float> const &);
  template Float sum(Array<Float> const &);
  template Float variance(Array<Float> const &);
  template Float variance(Array<Float> const &, Float);
  template void indgen(Array<Float> &);
  template void indgen(Array<Float> &, Float);
  template void indgen(Array<Float> &, Float, Float);
  template void max(Array<Float> &, Array<Float> const &, Array<Float> const &);
  template void max(Array<Float> &, Array<Float> const &, Float const &);
  template void min(Array<Float> &, Array<Float> const &, Array<Float> const &);
  template void min(Array<Float> &, Array<Float> const &, Float const &);
  template void minMax(Float &, Float &, Array<Float> const &);
  template void minMax(Float &, Float &, IPosition &, IPosition &, Array<Float> const &);
  template void minMax(Float &, Float &, IPosition &, IPosition &, Array<Float> const &, Array<Bool> const &);
  template void minMaxMasked(Float &, Float &, IPosition &, IPosition &, Array<Float> const &, Array<Float> const &);
  template void operator*=(Array<Float> &, Array<Float> const &);
  template void operator*=(Array<Float> &, Float const &);
  template void operator+=(Array<Float> &, Array<Float> const &);
  template void operator+=(Array<Float> &, Float const &);
  template void operator-=(Array<Float> &, Array<Float> const &);
  template void operator-=(Array<Float> &, Float const &);
  template void operator/=(Array<Float> &, Array<Float> const &);
  template void operator/=(Array<Float> &, Float const &);
  template Array<Int> abs(Array<Int> const &);
  template Array<Int> max(Array<Int> const &, Array<Int> const &);
  template Array<Int> min(Array<Int> const &, Array<Int> const &);
  template Array<Int> operator*(Array<Int> const &, Array<Int> const &);
  template Array<Int> operator*(Array<Int> const &, Int const &);
  template Array<Int> operator*(Int const &, Array<Int> const &);
  template Array<Int> operator+(Array<Int> const &, Int const &);
  template Array<Int> operator+(Array<Int> const &, Array<Int> const &);
  template Array<Int> operator-(Array<Int> const &);
  template Array<Int> operator/(Array<Int> const &, Int const &);
  template Int max(Array<Int> const &);
  template Int min(Array<Int> const &);
  template Int product(Array<Int> const &);
  template Int sum(Array<Int> const &);
  template void indgen(Array<Int> &);
  template void indgen(Array<Int> &, Int);
  template void indgen(Array<Int> &, Int, Int);
  template void max(Array<Int> &, Array<Int> const &, Array<Int> const &);
  template void min(Array<Int> &, Array<Int> const &, Array<Int> const &);
  template void minMax(Int &, Int &, Array<Int> const &);
  template void operator*=(Array<Int> &, Array<Int> const &);
  template void operator*=(Array<Int> &, Int const &);
  template void operator+=(Array<Int> &, Array<Int> const &);
  template void operator+=(Array<Int> &, Int const &);
  template void operator-=(Array<Int> &, Array<Int> const &);
  template void operator-=(Array<Int> &, Int const &);
  template void operator/=(Array<Int> &, Array<Int> const &);
  template void operator/=(Array<Int> &, Int const &);
  template Array<uChar> min<uChar>(Array<uChar> const &, Array<uChar> const &);
  template void min<uChar>(Array<uChar> &, Array<uChar> const &, Array<uChar> const &);
  template uChar max(Array<uChar> const &);
  template uChar min(Array<uChar> const &);
  template void indgen(Array<uChar> &);
  template void indgen(Array<uChar> &, uChar, uChar);
  template void minMax(uChar &, uChar &, Array<uChar> const &);
  template Array<uInt> operator*(Array<uInt> const &, uInt const &);
  template Array<uInt> operator-(Array<uInt> const &, uInt const &);
  template Array<uInt> operator/(Array<uInt> const &, uInt const &);
  template uInt max(Array<uInt> const &);
  template uInt min(Array<uInt> const &);
  template uInt sum(Array<uInt> const &);
  template void indgen(Array<uInt> &);
  template void indgen(Array<uInt> &, uInt);
  template void indgen(Array<uInt> &, uInt, uInt);
  template void minMax(uInt &, uInt &, Array<uInt> const &);
  template void operator*=(Array<uInt> &, uInt const &);
  template void operator+=(Array<uInt> &, uInt const &);
  template void operator-=(Array<uInt> &, uInt const &);
  template void operator/=(Array<uInt> &, uInt const &);
// 1130 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template Char max(Array<Char> const &);
  template Char min(Array<Char> const &);
  template void indgen(Array<Char> &);
  template void indgen(Array<Char> &, Char, Char);
  template void minMax(Char &, Char &, Array<Char> const &);
// 1140 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template Long max(Array<Long> const &);
  template Long min(Array<Long> const &);
  template void indgen(Array<Long> &);
  template void indgen(Array<Long> &, Long, Long);
  template void minMax(Long &, Long &, Array<Long> const &);
// 1150 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template Short max(Array<Short> const &);
  template Short min(Array<Short> const &);
  template void indgen(Array<Short> &);
  template void indgen(Array<Short> &, Short, Short);
  template void minMax(Short &, Short &, Array<Short> const &);
  template void operator+=(Array<Short> &, Short const &);
  template void operator-=(Array<Short> &, Short const &);
// 1160 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template uLong max(Array<uLong> const &);
  template uLong min(Array<uLong> const &);
  template void indgen(Array<uLong> &);
  template void indgen(Array<uLong> &, uLong, uLong);
  template void minMax(uLong &, uLong &, Array<uLong> const &);
// 1170 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template uShort max(Array<uShort> const &);
  template uShort min(Array<uShort> const &);
  template void indgen(Array<uShort> &);
  template void indgen(Array<uShort> &, uShort, uShort);
  template void minMax(uShort &, uShort &, Array<uShort> const &);
// 1180 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void convertArray(Array<Double> &, Array<Double> const &);
  template void convertArray(Array<Double> &, Array<Float> const &);
  template void convertArray(Array<Double> &, Array<Int> const &);
  template void convertArray(Array<Double> &, Array<Short> const &);
  template void convertArray(Array<Double> &, Array<uChar> const &);
  template void convertArray(Array<Double> &, Array<uInt> const &);
  template void convertArray(Array<Double> &, Array<uShort> const &);
  template void convertArray(Array<Float> &, Array<Double> const &);
  template void convertArray(Array<Float> &, Array<Int> const &);
  template void convertArray(Array<Float> &, Array<Short> const &);
  template void convertArray(Array<Float> &, Array<uShort> const & );
  template void convertArray(Array<Float> &, Array<uChar> const &);
  template void convertArray(Array<Float> &, Array<uInt> const &);
  template void convertArray(Array<Int> &, Array<Short> const &);
  template void convertArray(Array<Int> &, Array<uChar> const &);
  template void convertArray(Array<Int> &, Array<uInt> const &);
  template void convertArray(Array<Int> &, Array<uShort> const &);
  template void convertArray(Array<Int> &, Array<Bool> const &);
  template void convertArray(Array<Int> &, Array<Float> const &);
  template void convertArray(Array<Int> &, Array<Double> const &);
  template void convertArray(Array<Short> &, Array<Char> const &);
  template void convertArray(Array<Short> &, Array<Double> const &);
  template void convertArray(Array<Short> &, Array<Float> const &);
  template void convertArray(Array<Short> &, Array<Int> const &);
  template void convertArray(Array<Short> &, Array<Long> const &);
  template void convertArray(Array<Short> &, Array<Short> const &);
  template void convertArray(Array<Short> &, Array<uChar> const &);
  template void convertArray(Array<Short> &, Array<uInt> const &);
  template void convertArray(Array<Short> &, Array<uLong> const &);
  template void convertArray(Array<Short> &, Array<uShort> const &);
  template void convertArray(Array<uChar> &, Array<Int> const &);
  template void convertArray(Array<uChar> &, Array<Short> const &);
  template void convertArray(Array<uChar> &, Array<Float> const &);
  template void convertArray(Array<uChar> &, Array<Double> const &);
  template void convertArray(Array<uInt> &, Array<Int> const &);
  template void convertArray(Array<uInt> &, Array<Short> const &);
  template void convertArray(Array<uInt> &, Array<uChar> const &);
  template void convertArray(Array<uInt> &, Array<Float> const &);
  template void convertArray(Array<uInt> &, Array<Double> const &);
  template void convertArray(Array<uShort> &, Array<uChar> const &);
  template void convertArray(Array<uShort> &, Array<Short> const &);
  template void convertArray(Array<uShort> &, Array<Int> const &);
  template void convertArray(Array<uShort> &, Array<Float> const &);
  template void convertArray(Array<uShort> &, Array<Double> const &);
// 1190 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void operator*=(Array<Bool> &, Array<Bool> const &);
  template Bool sum(Array<Bool> const &);
// 1200 casa/Arrays/ArrayMath.cc casa/Arrays/Array.h 
  template void operator*=(Array<uInt> &, Array<uInt> const &);
  template void operator+=(Array<uInt> &, Array<uInt> const &);
  template void operator-=(Array<uInt> &, Array<uInt> const &);
  template void operator/=(Array<uInt> &, Array<uInt> const &);
// 1210 casa/Arrays/ArrayMath.cc casa/BasicSL/Complex.h 
  template Array<Double> partialMins(Array<Double> const &, IPosition const &);
  template Array<Double> partialMaxs(Array<Double> const &, IPosition const&);
  template Array<Double> partialSums(Array<Double> const &, IPosition const &);
  template Array<Double> partialProducts(Array<Double> const &, IPosition const &);
  template Array<Double> partialMeans(Array<Double> const &, IPosition const &);
  template Array<Double> partialVariances(Array<Double> const &, IPosition const &, Array<Double> const &);
  template Array<Double> partialAvdevs(Array<Double> const &, IPosition const &, Array<Double> const &);
  template Array<Double> partialMedians(Array<Double> const &, IPosition const &, Bool, Bool);
  template Array<Double> partialFractiles(Array<Double> const &, IPosition const &, Float, Bool);
  template Array<DComplex> partialSums(Array<DComplex> const &, IPosition const &);
  template Array<DComplex> partialProducts(Array<DComplex> const &, IPosition const &);

  template Array<Bool> concatenateArray(Array<Bool> const &, Array<Bool> const &);
  template Array<Int> concatenateArray(Array<Int> const &, Array<Int> const &);
  template Array<uInt> concatenateArray(Array<uInt> const &, Array<uInt> const &);
  template Array<Float> concatenateArray(Array<Float> const &, Array<Float> const &);
  template Array<Double> concatenateArray(Array<Double> const &, Array<Double> const &);
  template Array<Complex> concatenateArray(Array<Complex> const &, Array<Complex> const &);
  template Array<DComplex> concatenateArray(Array<DComplex> const &, Array<DComplex> const &);
  template Array<String> concatenateArray(Array<String> const &, Array<String> const &);

  template Array<uInt> reorderArray(Array<uInt> const &, IPosition const &, Bool);
  template Array<Int> reorderArray(Array<Int> const &, IPosition const &, Bool);
  template Array<Float> reorderArray(Array<Float> const &, IPosition const &, Bool);
  template Array<Double> reorderArray(Array<Double> const &, IPosition const &, Bool);
  template Array<Complex> reorderArray(Array<Complex> const &, IPosition const &, Bool);
  template Array<DComplex> reorderArray(Array<DComplex> const &, IPosition const &, Bool);

  // test/templates
  template Array<Int> cube(Array<Int> const &);
  template Array<Int> operator-(Array<Int> const &, Array<Int> const &);
  template Array<Int> operator-(Array<Int> const &, Int const &);
  template Array<Int> square(Array<Int> const &);
  template Int mean(Array<Int> const &);
  template Int median(Array<Int> const &, Bool);
  template Int median(Array<Int> const &, Bool, Bool, Bool);

  template class Cube<Matrix<CountedPtr<Random> > >;
  template class Cube<Complex>;
  template class Cube<DComplex>;
  template class Cube<Bool>;
  template class Cube<Double>;
  template class Cube<Float>;
  template class Cube<Int>;
  template class Cube<Short>;
  template class Cube<uInt>;

  template Array<Complex> & operator/=(Array<Complex> &, MaskedArray<Complex> const &);
  template Float min(MaskedArray<Float> const &);
  template Float max(MaskedArray<Float> const &);
  template Double min(MaskedArray<Double> const &);
  template Double max(MaskedArray<Double> const &);
  template MaskedArray<Float> abs(MaskedArray<Float> const &);
  template Float sum(MaskedArray<Float> const &);
  template Float mean(MaskedArray<Float> const &);
  template Array<Complex> & operator+=(Array<Complex> &, MaskedArray<Complex> const &);
  template const MaskedArray<Complex> & operator+=(MaskedArray<Complex> const &, Complex const &);
  template const MaskedArray<Complex> & operator+=(MaskedArray<Complex> const &, MaskedArray<Complex> const &);
  template const MaskedArray<Complex> & operator/=(MaskedArray<Complex> const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator+=(MaskedArray<Float> const &, Array<Float> const &);
  template Array<Float> & operator+=(Array<Float> &, MaskedArray<Float> const &);
  template const MaskedArray<Complex> & operator+=(MaskedArray<Complex> const &, Array<Complex> const &);
  template Float variance(MaskedArray<Float> const &);
  template Float variance(MaskedArray<Float> const &, Float);
  template MaskedArray<Float> operator-(MaskedArray<Float> const &, Float const &);
  template MaskedArray<Float> operator-(Float const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator*=(MaskedArray<Float> const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator-=(MaskedArray<Float> const &, Float const &);
  template const MaskedArray<Float> & operator-=(MaskedArray<Float> const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator+=(MaskedArray<Float> const &, Float const &);
  template const MaskedArray<Float> & operator+=(MaskedArray<Float> const &, MaskedArray<Float> const &);
  template const MaskedArray<Float> & operator/=(MaskedArray<Float> const &, MaskedArray<Float> const &);
  template const MaskedArray<Int> & operator+=(MaskedArray<Int> const &, Int const &);
  template MaskedArray<Int> operator-(MaskedArray<Int> const &);
  template MaskedArray<uInt> operator-(MaskedArray<uInt> const &);
  template void minMax(Float &, Float &, IPosition &, IPosition &, MaskedArray<Float> const &);
  template void minMax(Float &, Float &, MaskedArray<Float> const &);

  template class MaskedArray<Array<Double> >;
  template class MaskedArray<Slicer>;
  template class MaskedArray<Vector<Complex> >;
  template class MaskedArray<Vector<Double> >;
  template class MaskedArray<Vector<Float> >;
  template class MaskedArray<Bool>;
  template class MaskedArray<uChar>;
  template class MaskedArray<Short>;
  template class MaskedArray<uShort>;
  template class MaskedArray<Int>;
  template class MaskedArray<uInt>;
  template class MaskedArray<Float>;
  template class MaskedArray<Double>;
  template class MaskedArray<Complex>;
  template class MaskedArray<DComplex>;
  template class MaskedArray<String>;
  template class MaskedArray<MVDirection>;
  template class MaskedArray<MVFrequency>;
  template class MaskedArray<MVPosition>;
  template class MaskedArray<MVRadialVelocity>;
  template class MaskedArray<MVTime>;
  template class MaskedArray<Quantum<Double> >;
  template class MaskedArray<Quantum<Float> >;
  template class MaskedArray<QuantumHolder>;
  template class MaskedArray<Unit>;
  template class MaskedArray<Matrix<CountedPtr<Random> > >;
  template class MaskedArray<CountedPtr<Random> >;
  template class MaskedArray<Char>;
  template class MaskedArray<Float *>;
  template class MaskedArray<Long>;
  template class MaskedArray<uLong>;

  template class Matrix<Complex>;
  template class Matrix<DComplex>;
  template class Matrix<String>;
  template class Matrix<MVPosition>;
  template class Matrix<CountedPtr<Random> >;
  template class Matrix<Matrix<CountedPtr<Random> > >;
  template class Matrix<Bool>;
  template class Matrix<Char>;
  template class Matrix<Double>;
  template class Matrix<Float>;
  template class Matrix<Int>;
  template class Matrix<Long>;
  template class Matrix<Short>;
  template class Matrix<uChar>;
  template class Matrix<uInt>;
  template class Matrix<uLong>;
  template class Matrix<uShort>;

  template Matrix<Complex> product(Matrix<Complex> const &, Matrix<Complex> const &);
  template Matrix<Complex> transpose(Matrix<Complex> const &);
  template Matrix<DComplex> transpose(Matrix<DComplex> const &);
  template Double innerProduct(Vector<Double> const &, Vector<Double> const &);
  template Float innerProduct(Vector<Float> const &, Vector<Float> const &);
  template Int innerProduct(Vector<Int> const &, Vector<Int> const &);
  template Matrix<Bool> transpose(Matrix<Bool> const &);
  template Matrix<Double> transpose(Matrix<Double> const &);
  template Matrix<Float> transpose(Matrix<Float> const &);
  template Matrix<Int> transpose(Matrix<Int> const &);
  template Vector<Double> product(Matrix<Double> const &, Vector<Double> const &);
  template Vector<Float> product(Matrix<Float> const &, Vector<Float> const &);

  template class Vector<Slicer>;
  template class Vector<Vector<Complex> >;
  template class Vector<MVDirection>;
  template class Vector<MVFrequency>;
  template class Vector<MVPosition>;
  template class Vector<MVRadialVelocity>;
  template class Vector<MVTime>;
  template class Vector<QuantumHolder>;
  template class Vector<Unit>;
  template class Vector<Array<Double> >;
  template class Vector<Bool>;
  template class Vector<uChar>;
  template class Vector<Short>;
  template class Vector<uShort>;
  template class Vector<Int>;
  template class Vector<uInt>;
  template class Vector<Float>;
  template class Vector<Double>;
  template class Vector<Complex>;
  template class Vector<DComplex>;
  template class Vector<String>;
  template class Vector<Quantum<Double> >;
  template class Vector<Matrix<CountedPtr<Random> > >;
  template class Vector<CountedPtr<Random> >;
  template class Vector<Char>;
  template class Vector<Float *>;
  template class Vector<Long>;
  template class Vector<Vector<Double> >;
  template class Vector<Vector<Float> >;
  template class Vector<uLong>;

  AIPS_VECTOR2_AUX_TEMPLATES(Int);
  AIPS_VECTOR2_AUX_TEMPLATES(Bool);
  AIPS_VECTOR2_AUX_TEMPLATES(uChar);
  AIPS_VECTOR2_AUX_TEMPLATES(Short);
  AIPS_VECTOR2_AUX_TEMPLATES(uShort);
  AIPS_VECTOR2_AUX_TEMPLATES(uInt);
  AIPS_VECTOR2_AUX_TEMPLATES(Float);
  AIPS_VECTOR2_AUX_TEMPLATES(Double);
  AIPS_VECTOR2_AUX_TEMPLATES(Complex);
  AIPS_VECTOR2_AUX_TEMPLATES(DComplex);
  AIPS_VECTOR2_AUX_TEMPLATES(String);

  template class ReadOnlyVectorIterator<Complex>;
  template class VectorIterator<Complex>;
  template class ReadOnlyVectorIterator<DComplex>;
  template class VectorIterator<DComplex>;
  template class ReadOnlyVectorIterator<Bool>;
  template class VectorIterator<Bool>;
  template class ReadOnlyVectorIterator<Double>;
  template class VectorIterator<Double>;
  template class ReadOnlyVectorIterator<Float>;
  template class VectorIterator<Float>;
  template class ReadOnlyVectorIterator<uChar>;
  template class VectorIterator<uChar>;
  template class ReadOnlyVectorIterator<uInt>;
  template class VectorIterator<uInt>;
  template class ReadOnlyVectorIterator<uLong>;
  template class VectorIterator<uLong>;
  template class ReadOnlyVectorIterator<uShort>;
  template class VectorIterator<uShort>;
  template class VectorIterator<Int>;

}
