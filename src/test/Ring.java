/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package test;

import java.math.BigInteger;

/**
 *
 * @author nc
 */
public interface Ring<T> {
  /* each implementation of this should verify that both e1 and e2 are elements
   * of the ring */
  T add(T e1, T e2);
  T mult(T e1, T e2);
  T zero();
  T one();
  BigInteger nElems();
}
