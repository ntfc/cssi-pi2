/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package test;

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
  public PolynomialF2 mult(PolynomialF2 e1, PolynomialF2 e2) {
    throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
  }

  @Override
  public PolynomialF2 zero() {
    throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
  }

  @Override
  public PolynomialF2 one() {
    throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
  }

}
