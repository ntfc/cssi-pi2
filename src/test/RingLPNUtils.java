/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package test;

import java.math.BigInteger;
import java.util.Arrays;
import java.util.BitSet;

/**
 *
 * @author nc
 */
public class RingLPNUtils {
  private int securityParameter;
  /**
   * Creates a new empty instance of <code>RingLPNUtils</code>.
   */
  public RingLPNUtils(int secParam) {
    this.securityParameter = secParam;
  }

  // pi(c) function, for the irreductive case
  public BitSet mappingIrreductive(BigInteger c) {
    // convert biginteger to bitstring
    String bitString = c.toString(2);
    // pad bitstring, if necessary
    if(c.bitLength() < 80) {
      StringBuilder s = new StringBuilder();
      for(int i = 0; i < (80 - c.bitLength()); i++)
        s.append('0');
      bitString = s.toString() + bitString;
    }
    // divide c into 16 c's
    String[] cDivided = new String[16];
    for(int i = 0; i < 16; i++) {
      cDivided[i] = bitString.substring(i*5, (i*5)+5);
    }
    // create the polynomial v
    BitSet v = new BitSet();
    for(int j = 0; j < 16; j++) {
      int cj = Integer.parseInt(cDivided[j], 2);
      int i = 16 * j + cj; // in the paper, j ranges from 1..16 and not from 0..15
      v.set(i);
    }
    return v;

  }

}
