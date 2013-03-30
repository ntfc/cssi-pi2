/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package test;

import java.math.BigInteger;
import java.util.BitSet;

/**
 *
 * @author nc
 */
public class PolynomialRingF2 implements Ring<PolynomialF2> {
  private PolynomialF2 mod;
  public PolynomialRingF2(PolynomialF2 f) {
    this.mod = f;
  }

  @Override
  public PolynomialF2 add(PolynomialF2 e1, PolynomialF2 e2) {
    throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
  }

  @Override
  public PolynomialF2 mult(PolynomialF2 a, PolynomialF2 b) {

    return new PolynomialF2(rotateRight(b.getCoeficients(), a.getCoeficients().cardinality()));

  }
  // it doesnt change the BitSet bs!
  private BitSet rotateRight(BitSet bs, int n) {
    int length = bs.length();
    BitSet bsClone = (BitSet) bs.clone();
    for(int it = 0; it < n; it++) {
      // Get the last bit
      boolean bLast = bsClone.get(length - 1);
      // get the first bs.length()-1 bits
      BitSet b = bsClone.get(0, length-1);
      // clear the old bsClone
      bsClone.clear();
      // the new first bit is the same as the old last bit
      bsClone.set(0, bLast);
      // the next new bits are 1 position ahead of the old bits
      for(int i = b.nextSetBit(0); i >= 0; i = b.nextSetBit(i+1))
        bsClone.set(i+1);
    }
    return bsClone;
  }

  @Override
  public PolynomialF2 zero() {
    throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
  }

  @Override
  public PolynomialF2 one() {
    throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
  }

  // TC.pdf, pagina 153, capitulo 6 anéis
  @Override
  public BigInteger nElems() {
    return BigInteger.valueOf(2).pow(this.mod.degree());
  }

  
}
