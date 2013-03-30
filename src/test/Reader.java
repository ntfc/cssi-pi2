/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package test;

import java.math.BigInteger;
import java.security.SecureRandom;

/**
 *
 * @author nc
 */
public class Reader {
  private int securityParameter;
  private BigInteger c;
  /**
   * Creates a new empty instance of <code>Reader</code>.
   */
  public Reader(int secutiryParam) {
    this.securityParameter = secutiryParam;
  }

  public BigInteger genC() {
    this.c = new BigInteger(this.securityParameter, new SecureRandom());
    return this.c;
  }
  public void calcE() {
    // TODO: e' = z - r(s * mapping(c) + s')
  }
}
