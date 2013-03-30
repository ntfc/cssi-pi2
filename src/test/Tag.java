/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package test;

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

  public void genR() {
    // TODO
  }
  public void genE() {
    // TODO
  }
  public void calcZ() {
    // TODO: z = r * (s * mapping(c) + s') + e
  }

}
