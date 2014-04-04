class Vec3{
  public float x;
  public float y;
  public float z;
  Vec3() {
    x = 0;
    y = 0;
    z = 0;
  }
  Vec3(float vx, float vy, float vz) {
    x = vx;
    y = vy;
    z = vz;
  }
}
    

class Letter{
  
  public ArrayList strokePoints;
  
  Letter(){
    strokePoints = new ArrayList();
  }
  
  public void addPoint(float x, float y, float z) {
    Vec3 v = new Vec3();
    v.x = x;
    v.y = y;
    v.z = z;
    strokePoints.add( v );
  }
  // Getters
  public Vec3 getPoint(int index) {
    return (Vec3)strokePoints.get(index);
  }
  public int getTotal() {
    return strokePoints.size();
  }
  
}

class Alphabet{
  
  public ArrayList abcs;
  public ArrayList letters;
  
  Alphabet(){
    abcs = new ArrayList();
    letters = new ArrayList();
  }
  public void addLetter(String name, float[][] coords, int total) {
    //println("Add letter: " + name);
    abcs.add(name);
    Letter nLetter = new Letter();
    for(int i = 0; i < total; ++i) {
      nLetter.addPoint( coords[i][0], coords[i][1], coords[i][2] );
    }
    letters.add(nLetter);
  }
  // Getters
  public Letter getLetter(String letter) {

    int i, total = abcs.size();
    for(i = 0; i < total; ++i) {
      if (abcs.get(i).equals(letter) == true) {
        //println("Found letter: " + letter);
        return (Letter)letters.get(i);
      } else {
        //println("Letter Not Found: " + letter);
      }
    }
    return null;
  }
}



