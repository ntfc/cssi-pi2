/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package test;

import java.util.BitSet;

/**
 *
 * @author nc
 */
public class PolynomialF2 {
  private BitSet coeficients;
  /**
   * Creates a new empty instance of <code>PolynomialF2</code>.
   */
  // TODO: creat superclasse PolynomialF2
  // 1 + x == PolynomialF2(0, 1)
  // 1 + x^2 == PolynomialF2(0, 2)
  // 1 + x^2 + x^10 == PolynomialF2(0, 2, 10)
  public PolynomialF2(int ... d) throws IndexOutOfBoundsException {
    this.coeficients = new BitSet();
    for(int dg : d) {
      coeficients.set(dg);
    }
  }
  public PolynomialF2(BitSet bs) {
    this.coeficients = bs;
  }

  public BitSet getCoeficients() {
    return coeficients;
  }

  // return the degree of this polynomial
  public int degree() {
    if(this.coeficients.length() == 0)
      return 0;
    else
      return this.coeficients.length() - 1;
  }
  
  @Override
  public String toString() {
    StringBuilder s = new StringBuilder();
    if(coeficients.isEmpty()) return s.append(0).toString();
    if(degree() == 0)
      s.append(coeficients.get(0) ? '1' : '0');
    if(degree() == 1) {
      if(coeficients.get(0))
        s.append(coeficients.get(0) ? '1' : '0').append(" + ");
      s.append("x");
    }
    if(degree() > 1) {
      // print first x
      if(coeficients.get(0)) s.append("1 + ");
      if(coeficients.get(1)) s.append("x + ");
      // print remaining x's, except the last
      for(int i = 2; i < degree(); i++) {
        boolean m = coeficients.get(i);
        if(m) {
          s.append("x^").append(i).append(" + ");
        }
      }
      // print last x
      s.append("x^").append(degree());
    }
    return s.toString();
  }

  
  // return c = this + b;
  public PolynomialF2 add(PolynomialF2 b) throws PolynomialF2Exception {
    PolynomialF2 c = new PolynomialF2((BitSet) this.coeficients.clone());
    c.coeficients.xor(b.coeficients);
    return c;
    
  }

  // return c = this * b;
  public PolynomialF2 mul(PolynomialF2 b) {
    BitSet c = new BitSet();
    for(int i = 0; i < this.coeficients.length(); i++) {
      for(int j = 0; j < b.coeficients.length(); j++) {
        int c_ij = c.get(i + j) ? 1 : 0;
        int n1 = this.coeficients.get(i) ? 1 : 0;
        int n2 = b.coeficients.get(j) ? 1 : 0;
        int new_c_ij = c_ij + (n1 * n2);
        c.set(i+j, (new_c_ij % 2) == 1);
      }
    }
    return new PolynomialF2(c);
  }
  /*
  // daqui para a baixo esta a multiplicação em módulo, QUASE
  // TODO: acho que isto esta mal. ver melhor..
  public PolynomialF2 mul(PolynomialF2 b) {
    return new PolynomialF2(rotateRight(b.coeficients, this.coeficients.cardinality()));
    
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
  */
}
