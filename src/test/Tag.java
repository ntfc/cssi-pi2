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
public class Tag {
  private int securityParameter;
  // TODO: secret key e s,s'
  /**
   * Creates a new empty instance of <code>Tag</code>.
   */
  public Tag(int securityParam) {
    this.securityParameter = securityParam;
  }

  //estará isto certo? A estar talvez devessemos repensar a ideia de usarmos bitset no polinomio
  public double[] genE(PolynomialF2 p, double t) {
    // TODO
    BitSet bs = p.getCoeficients();
    double[] e = new double[bs.length()];
    
    for(int i = 0; i<bs.length(); i++){
      if(bs.get(i) == true)
        e[i] = t;
      else e[i] = 1-t;
    }
    
    return e;
  }
  
  //será probability mass function ou comulative distribution function? Usei a 2ª
  //http://en.wikipedia.org/wiki/Uniform_distribution_(discrete)
  public double[] genR(PolynomialF2 p) {
    // TODO
    BitSet bs = p.getCoeficients();
    double[] r = new double[bs.length()];
    
    for(int i = 0; i<bs.length(); i++){
      int val = bs.get(i) ? 1 : 0;//bool to int
      r[i] = (val - 0 + 1)/(1 - 0 + 1);
    }
    
    return r;
  }
  
  public void calcZ() {
    // TODO: z = r * (s * mapping(c) + s') + e
  }

}
