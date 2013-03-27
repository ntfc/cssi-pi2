/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package test;

import java.util.Arrays;
import java.util.BitSet;

/**
 *
 * @author nc
 */
public class Test {

  /**
   * Creates a new empty instance of <code>Test</code>.
   */
  public static void main(String[] args) throws Exception {
    PolynomialF2 p1 = new PolynomialF2(1,2);
    PolynomialF2 p2 = new PolynomialF2(0,1,3);
    PolynomialF2 p3 = new PolynomialF2(0,1,2);
    System.out.println("'"+p1+"'");
    System.out.println("'"+p2+"'");
    System.out.println("'"+p3+"'");
    System.out.println("p1+p2="+p1.add(p2).toString());
    System.out.println("p1*p2="+p1.mul(p2).toString());
    System.out.println("p1+p3="+p1.add(p3).toString());
    System.out.println("p2*p2="+p2.mul(p2).toString());


  }

}
