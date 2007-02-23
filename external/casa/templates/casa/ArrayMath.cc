
// include of header files 

#include <casa/Arrays/Array.h>
#include <casa/BasicSL/Complex.h>

// include of implementation files

#include <casa/Arrays/ArrayMath.cc>
#include <casa/Arrays/MatrixMath.cc>

namespace casa {

  // --------------------------------------------------------------- convertArray

  template void convertArray(Array<DComplex> &, Array<Complex> const &);
  template void convertArray(Array<DComplex> &, Array<DComplex> const &);
  template void convertArray(Array<DComplex> &, Array<Double> const &);
  template void convertArray(Array<DComplex> &, Array<Float> const &);
  template void convertArray(Array<DComplex> &, Array<Int> const &);
  template void convertArray(Array<DComplex> &, Array<Short> const &);
  template void convertArray(Array<DComplex> &, Array<uChar> const &);
  template void convertArray(Array<DComplex> &, Array<uInt> const &);

  template void convertArray(Array<Complex> &, Array<DComplex> const &);
  template void convertArray(Array<Complex> &, Array<Double> const &);
  template void convertArray(Array<Complex> &, Array<Float> const &);
  template void convertArray(Array<Complex> &, Array<Int> const &);
  template void convertArray(Array<Complex> &, Array<Short> const &);
  template void convertArray(Array<Complex> &, Array<uChar> const &);
  template void convertArray(Array<Complex> &, Array<uInt> const &);

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

  template void convertArray(Array<Bool> &, Array<Int> const &);

  // --------------------------------------------------------------- crossProduct

  template Vector<Int> crossProduct(Vector<Int> const&, Vector<Int> const&);
  template Vector<uInt> crossProduct(Vector<uInt> const&, Vector<uInt> const&);
  template Vector<Short> crossProduct(Vector<Short> const&, Vector<Short> const&);
  template Vector<uShort> crossProduct(Vector<uShort> const&, Vector<uShort> const&);
  template Vector<Long> crossProduct(Vector<Long> const&, Vector<Long> const&);
  template Vector<uLong> crossProduct(Vector<uLong> const&, Vector<uLong> const&);
  template Vector<Float> crossProduct(Vector<Float> const&, Vector<Float> const&);
  template Vector<Double> crossProduct(Vector<Double> const&, Vector<Double> const&);
  template Vector<Complex> crossProduct(Vector<Complex> const&, Vector<Complex> const&);
  template Vector<DComplex> crossProduct(Vector<DComplex> const&, Vector<DComplex> const&);

  // -------------------------------------------------------------- directProduct

  template Matrix<Int> directProduct(Matrix<Int> const &, Matrix<Int> const &);
  template Matrix<uInt> directProduct(Matrix<uInt> const &, Matrix<uInt> const &);
  template Matrix<Short> directProduct(Matrix<Short> const &, Matrix<Short> const &);
  template Matrix<uShort> directProduct(Matrix<uShort> const &, Matrix<uShort> const &);
  template Matrix<Long> directProduct(Matrix<Long> const &, Matrix<Long> const &);
  template Matrix<uLong> directProduct(Matrix<uLong> const &, Matrix<uLong> const &);
  template Matrix<Float> directProduct(Matrix<Float> const &, Matrix<Float> const &);
  template Matrix<Double> directProduct(Matrix<Double> const &, Matrix<Double> const &);
  template Matrix<Complex> directProduct(Matrix<Complex> const &, Matrix<Complex> const &);
  template Matrix<DComplex> directProduct(Matrix<DComplex> const &, Matrix<DComplex> const &);

  // --------------------------------------------------------------- innerProduct
  
  template Int innerProduct(Vector<Int> const &, Vector<Int> const &);
  template uInt innerProduct(Vector<uInt> const &, Vector<uInt> const &);
  template Short innerProduct(Vector<Short> const &, Vector<Short> const &);
  template uShort innerProduct(Vector<uShort> const &, Vector<uShort> const &);
  template Float innerProduct(Vector<Float> const &, Vector<Float> const &);
  template Double innerProduct(Vector<Double> const &, Vector<Double> const &);
  
  // ------------------------------------------------------------------------ max

  template void max (Array<Int> &, Array<Int> const &, Int const &);
  template void max (Array<uInt> &, Array<uInt> const &, uInt const &);
  template void max (Array<Short> &, Array<Short> const &, Short const &);
  template void max (Array<uShort> &, Array<uShort> const &, uShort const &);
  template void max (Array<Long> &, Array<Long> const &, Long const &);
  template void max (Array<uLong> &, Array<uLong> const &, uLong const &);
  template void max (Array<Float> &, Array<Float> const &, Float const &);
  template void max (Array<Double> &, Array<Double> const &, Double const &);
  template void max (Array<Complex> &, Array<Complex> const &, Complex const &);
  template void max (Array<DComplex> &, Array<DComplex> const &, DComplex const &);

  template void max (Array<Int> &, Array<Int> const &, Array<Int> const &);
  template void max (Array<Float> &, Array<Float> const &, Array<Float> const &);
  template void max (Array<Double> &, Array<Double> const &, Array<Double> const &);
  template void max (Array<Complex> &, Array<Complex> const &, Array<Complex> const &);
  template void max (Array<DComplex> &, Array<DComplex> const &, Array<DComplex> const &);

  template Int max (Array<Int> const &);
  template uInt max (Array<uInt> const &);
  template Short max(Array<Short> const &);
  template uShort max(Array<uShort> const &);
  template Long max(Array<Long> const &);
  template uLong max(Array<uLong> const &);
  template Float max (Array<Float> const &);
  template Double max (Array<Double> const &);
  template Complex max (Array<Complex> const &);
  template DComplex max (Array<DComplex> const &);
  template Char max(Array<Char> const &);
  template uChar max(Array<uChar> const &);

  template Array<Int> max(Array<Int> const &, Int const &);
  template Array<Float> max(Array<Float> const &, Float const &);
  template Array<Double> max(Array<Double> const &, Double const &);
  template Array<Complex> max(Array<Complex> const &, Complex const &);
  template Array<DComplex> max(Array<DComplex> const &, DComplex const &);

  template Array<Int> max(Array<Int> const &, Array<Int> const &);
  template Array<Float> max(Array<Float> const &, Array<Float> const &);
  template Array<Double> max(Array<Double> const &, Array<Double> const &);
  template Array<Complex> max(Array<Complex> const &, Array<Complex> const &);
  template Array<DComplex> max(Array<DComplex> const &, Array<DComplex> const &);

  // ------------------------------------------------------------------------ min

  template void min (Array<Int> &, Array<Int> const &, Int const &);
  template void min (Array<Float> &, Array<Float> const &, Float const &);
  template void min (Array<Double> &, Array<Double> const &, Double const &);
  template void min (Array<Complex> &, Array<Complex> const &, Complex const &);
  template void min (Array<DComplex> &, Array<DComplex> const &, DComplex const &);

  template void min (Array<Int> &, Array<Int> const &, Array<Int> const &);
  template void min (Array<Float> &, Array<Float> const &, Array<Float> const &);
  template void min (Array<Double> &, Array<Double> const &, Array<Double> const &);
  template void min (Array<Complex> &, Array<Complex> const &, Array<Complex> const &);
  template void min (Array<DComplex> &, Array<DComplex> const &, Array<DComplex> const &);

  template Int min (Array<Int> const &);
  template uInt min (Array<uInt> const &);
  template Short min(Array<Short> const &);
  template uShort min (Array<uShort> const &);
  template Long min (Array<Long> const &);
  template uLong min(Array<uLong> const &);
  template Float min (Array<Float> const &);
  template Double min (Array<Double> const &);
  template Complex min (Array<Complex> const &);
  template DComplex min (Array<DComplex> const &);
  template Char min (Array<Char> const &);
  template uChar min (Array<uChar> const &);

  template Array<Int> min (Array<Int> const &, Int const &);
  template Array<Float> min (Array<Float> const &, Float const &);
  template Array<Double> min (Array<Double> const &, Double const &);
  template Array<Complex> min (Array<Complex> const &, Complex const &);
  template Array<DComplex> min (Array<DComplex> const &, DComplex const &);

  template Array<Int> min (Array<Int> const &, Array<Int> const &);
  template Array<Float> min (Array<Float> const &, Array<Float> const &);
  template Array<Double> min (Array<Double> const &, Array<Double> const &);
  template Array<Complex> min (Array<Complex> const &, Array<Complex> const &);
  template Array<DComplex> min (Array<DComplex> const &, Array<DComplex> const &);

  // -------------------------------------------------------------------- product
  
  template Int product(Array<Int> const &);
  template uInt product(Array<uInt> const &);
  template Short product(Array<Short> const &);
  template uShort product(Array<uShort> const &);
  template Long product(Array<Long> const &);
  template uLong product(Array<uLong> const &);
  template Float product(Array<Float> const &);
  template Double product(Array<Double> const &);
  template Complex product(Array<Complex> const &);
  template DComplex product(Array<DComplex> const &);
  
  template Matrix<Int> product(Matrix<Int> const &, Matrix<Int> const &);
  template Matrix<uInt> product(Matrix<uInt> const &, Matrix<uInt> const &);
  template Matrix<Short> product(Matrix<Short> const &, Matrix<Short> const &);
  template Matrix<uShort> product(Matrix<uShort> const &, Matrix<uShort> const &);
  template Matrix<Long> product(Matrix<Long> const &, Matrix<Long> const &);
  template Matrix<uLong> product(Matrix<uLong> const &, Matrix<uLong> const &);
  template Matrix<Float> product(Matrix<Float> const &, Matrix<Float> const &);
  template Matrix<Double> product(Matrix<Double> const &, Matrix<Double> const &);
  template Matrix<Complex> product(Matrix<Complex> const &, Matrix<Complex> const &);
  template Matrix<DComplex> product(Matrix<DComplex> const &, Matrix<DComplex> const &);

  template Vector<Int> product(Matrix<Int> const &, Vector<Int> const &);
  template Vector<uInt> product(Matrix<uInt> const &, Vector<uInt> const &);
  template Vector<Short> product(Matrix<Short> const &, Vector<Short> const &);
  template Vector<uShort> product(Matrix<uShort> const &, Vector<uShort> const &);
  template Vector<Long> product(Matrix<Long> const &, Vector<Long> const &);
  template Vector<uLong> product(Matrix<uLong> const &, Vector<uLong> const &);
  template Vector<Float> product(Matrix<Float> const &, Vector<Float> const &);
  template Vector<Double> product(Matrix<Double> const &, Vector<Double> const &);
  template Vector<Complex> product(Matrix<Complex> const &, Vector<Complex> const &);
  template Vector<DComplex> product(Matrix<DComplex> const &, Vector<DComplex> const &);

  template Matrix<Int> product(Vector<Int> const &, Matrix<Int> const &);
  template Matrix<uInt> product(Vector<uInt> const &, Matrix<uInt> const &);
  template Matrix<Short> product(Vector<Short> const &, Matrix<Short> const &);
  template Matrix<uShort> product(Vector<uShort> const &, Matrix<uShort> const &);
  template Matrix<Long> product(Vector<Long> const &, Matrix<Long> const &);
  template Matrix<uLong> product(Vector<uLong> const &, Matrix<uLong> const &);
  template Matrix<Float> product(Vector<Float> const &, Matrix<Float> const &);
  template Matrix<Double> product(Vector<Double> const &, Matrix<Double> const &);
  template Matrix<Complex> product(Vector<Complex> const &, Matrix<Complex> const &);
  template Matrix<DComplex> product(Vector<DComplex> const &, Matrix<DComplex> const &);

  // ------------------------------------------------------------------ transpose

  template Matrix<Bool> transpose(Matrix<Bool> const &);
  template Matrix<Int> transpose(Matrix<Int> const &);
  template Matrix<uInt> transpose(Matrix<uInt> const &);
  template Matrix<Short> transpose(Matrix<Short> const &);
  template Matrix<Float> transpose(Matrix<Float> const &);
  template Matrix<Double> transpose(Matrix<Double> const &);
  template Matrix<Complex> transpose(Matrix<Complex> const &);
  template Matrix<DComplex> transpose(Matrix<DComplex> const &);
}
