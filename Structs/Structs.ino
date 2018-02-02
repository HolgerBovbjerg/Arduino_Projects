
struct Members {
  char their_name[20];
  char hometown[20];
  int age;
};

struct Members var1, var2, var3;

void print_info( struct Members member )
{
Serial.print("Navn : "); Serial.println(member.their_name);
Serial.print("Hjemby : "); Serial.println(member.hometown);
Serial.print("Alder : "); Serial.println(member.age);
}

void setup() {
  Serial.begin(9600);
  var1.their_name = "Andreas";
  var1.hometown = "Horsens";
  var1.age = 23;
  var2.their_name = "Leon";
  var2.hometown = "Skiby";
  var2.age = 19;
  var3.their_name = "Morten";
  var3.hometown = "Aalborg";
  var3.age = 28;
}

void loop() {
  print_info(var1);
  print_info(var2);
  print_info(var3);
  delay(2000);
}
