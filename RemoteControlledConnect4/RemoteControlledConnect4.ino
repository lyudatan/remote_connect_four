#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>




//MAC address and IP address
byte mac[] = {0xFF, 0x7A, 0x2F, 0xA5, 0xDE, 0xA2};
IPAddress ip(169, 254, 108, 11);  // ip in lan assigned to arduino
byte server[] = {169, 254, 108, 10}; // numeric server IP address

EthernetClient client;

int s0 = 0;      //select pin at the 4051 (s0)
int s1 = 0;      //select pin at the 4051 (s1)
int s2 = 0;      //select pin at the 4051 (s2)
int y = 0;       //which y pin we are selecting

int playerTurn = 1; //who plays first. Used to alternate between players
int player1 =1;     //returns player that won
int player2 =2;     //returns player that won
int win =0;
int board [6][7];     // 6 rows by 7 columns board
int sensorValue = 0;  // value read from sensors
int countMoves =0;    // incremnted every move

boolean sent = false;

int currentPosition = 0;  // position of stepper motor
// sequence to drive the stepper with 8 pulses for each step
const int stepSequance[8] = {B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001};

Servo servoDrop; // Define servo object

void setup(){
  
   Serial.begin(9600);  //serial monitor for testing
   
  // disable the SD card by switching pin 4 high
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
   //set 2, 3, 5, 6 as outputs to stepper
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  
  //set pins 2, 3, 5 as output to multiplexer
  pinMode(7, OUTPUT);    // s0
  pinMode(8, OUTPUT);    // s1
  pinMode(9, OUTPUT);    // s2
  
  
  // set analogue A3 as digital output 
  pinMode(16,OUTPUT);    
  servoDrop.attach(16); // attach servo on digital pin 17
  
  //LED to indicate player turn and end of game
  pinMode(17,OUTPUT); 
  pinMode(18,OUTPUT); 
  pinMode(19,OUTPUT); 
  
  resetArray();           //reset the array to 0 at start

  
  // start the Ethernet connection:
  //if (Ethernet.begin(mac) == 0) {
    //Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address manualy:
    Ethernet.begin(mac, ip);
 // }
  // wait 1 second to initialize Ethernet:
  delay(1000);
  //Serial.println("connecting...");

  
}

void loop () {
  if(win==0){
    
  //Serial.println("void loop");
  //go through all sensors
  while(playerTurn==1){
    for (y=0; y<=6; y++) {

      // select the bit  
      s0 = bitRead(y,0);        
      s1 = bitRead(y,1);         
      s2 = bitRead(y,2);         
    
      digitalWrite(7, s0);
      digitalWrite(8, s1);
      digitalWrite(9, s2);

     // read the input on analog pin 0:
     sensorValue = analogRead(A0);
     checkForInput();

     delay(10);        // delay in between reads
  
    }  
  }
  
  if(playerTurn=2){
    
    //connect to server
    receiveData();
    delay(1000); //check for new data every second
  }
  }
  else if(win==1){
  Serial.println("Player 1 won");
  digitalWrite(18, HIGH); //Indicate Player 1 won
  }
  else if(win==2){
  Serial.println("Player 2 won");
  digitalWrite(19, HIGH); //Indicate Player 2 won
  }
  else{
  win=0;
  }
}

void  resetArray(){    //assign all values in the array to 0
  for (int i = 0; i < 6; i++){
    for (int j = 0; j < 7; j++){
      board[i][j] = 0;
    }
  }
}

void storeInArray(int column){  //store values in array depending on player
//Serial.println("");
//Serial.println("store in array");
//Serial.println(column);
//Serial.println(playerTurn);
  for (int row = 5; row >= 0; row--){
      if (board[row][column] == 0){
        board[row][column] = playerTurn;
        if(playerTurn==1){   //alternate between players
        playerTurn=2;}
        else if(playerTurn==2){
        playerTurn=1;}
        break;
      } 
  }
  
  win = checkForWin(); //check for win
    
  boardIsFull();       //check for draw
  
  /*print board used for testing
  for (int i = 0; i < 6; i++){
    for (int j = 0; j < 7; j++){
     Serial.print(board[i][j]); 
    }
    Serial.println("");
  }*/
  
}


int checkForWin(){
  //Serial.println("check for win");
  for (int i = 0; i < 6; i++){ //horizontal check
    for (int j = 0; j < 4; j++){
      if((board[i][j] == 1) && (board[i][j+1] == 1) && (board[i][j+2] == 1) && (board[i][j+3] == 1)){
      return player1;
      break;
      }
      else if((board[i][j] == 2) && (board[i][j+1] == 2) && (board[i][j+2] == 2) && (board[i][j+3] == 2)){
      return player2;
      break;
      }
    }
  }
  for (int k=0; k<6; k++){  //vertical check
    for (int l=0; l<3; l++){
     if((board[l][k] == 1) && (board[l+1][k] == 1) && (board[l+2][k] == 1) && (board[l+3][k] == 1)){
     return player1;
     break;
     }
     else if((board[l][k] == 2) && (board[l+1][k] == 2) && (board[l+2][k] == 2) && (board[l+3][k] == 2)){
     return player2;
     break;
     } 
    }
  } 
  for (int m=0; m < 3; m++){ //diagonal win "\"
    for (int n=0; n < 4; n++){
      if((board[m][n] == 1) && (board[m+1][n+1] == 1) && (board[m+2][n+2] == 1) && (board[m+3][n+3] == 1)){
      return player1;
      break;
      }
      else if((board[m][n] == 2) && (board[m+1][n+1] == 2) && (board[m+2][n+2] == 2) && (board[m+3][n+3] == 2)){
      return player2;
      break;
      }
    }
  }  
  for (int o=0; o<3; o++){  //diagonal check "/"
    for (int p=3; p<7; p++){
     if((board[o][p] == 1) && (board[o+1][p-1] == 1) && (board[o+2][p-2] == 1) && (board[o+3][p-3] == 1)){
     return player1;
     break;
     }
     else if((board[o][p] == 2) && (board[o+1][p-1] == 2) && (board[o+2][p-2] == 2) && (board[o+3][p-3] == 2)){
     return player2;
     break;
     } 
    }
  }
  return 0;
}

void checkForInput(){
  //Serial.println("check for input");
  if (sensorValue < 500){   //check if disk is dropped
    while (sensorValue <700){  //check if the disk had passed through the sensor
    sensorValue = analogRead(A0);
    }
    
    storeInArray(y);  //store new data
    
    sendData();      // send new data to server
  }
}


void boardIsFull(){
  //Serial.println("Board is full");
  countMoves++;
  if (countMoves > 42){
  endDraw();
  }
}

void endDraw(){
  //indicate that the game ended in a draw
  digitalWrite(17, HIGH);
  Serial.println("Draw");
}

void receiveData(){
  
  int remoteColumn = 0;
  int remoteCountMoves = 0;
  // listen for incoming clients
  if (client.connect(server, 80)) {
  Serial.print("connected... ");
   // Make a HTTP request:
   client.println("GET /displayData.php?");
   client.println( " HTTP/1.1");
   client.println( "Host: 192.168.0.2" );//web server
   client.println( "Content-Type: application/x-www-form-urlencoded" );
   client.println( "Connection: close" );
   client.println();
  } 
  
  else {
  Serial.println("connection failed");
  }
  //search for remote player move
  if (client.connected()) {
    if(client.find("</tr><tr><td>")){

      remoteColumn = client.parseInt();
      Serial.print("column is " );
      Serial.print(remoteColumn);
      
    if(client.find("</td><td>")){

      remoteCountMoves = client.parseInt();
      Serial.print("move is " );
      Serial.print(remoteCountMoves);
  
      }
    }
      else
      Serial.println("result not found");
      client.stop();
      delay(1000); 
  }
  
  else {
  Serial.println();
  Serial.println("not connected");
  client.stop();
  delay(1000);
  }
//client.stop();
//client.flush();
  if(remoteCountMoves==countMoves+1){

    autoInput(remoteColumn); //call stepper function with column number   
    //Serial.println(remoteColumn);
    //Serial.println(remoteCountMoves);
    storeInArray(remoteColumn);
    
  }
}


void sendData() //client function to send data to server
{
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
   client.print( "GET /mysql_connect.php?");
   client.print("countMove=");
   client.print(countMoves);
   client.print("&&");
   client.print("column=");
   client.print(y);
   client.println( " HTTP/1.1");
   client.println( "Host: 192.168.0.2" );
   client.println( "Content-Type: application/x-www-form-urlencoded" );
   client.println( "Connection: close" );
   client.println();
  } 
  else {
    Serial.println("connection failed");
    Serial.println();
    sendData();
  }

  // read incoming bytes from server
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

  }
}

void autoInput(int remoteColumn){
  
  int moveDistance = 0;
  
  if (currentPosition == remoteColumn){
    dropDisk();
  }
  //check direction
  else if(currentPosition < remoteColumn){
    moveDistance = remoteColumn-currentPosition;
    clockwise(moveDistance);
    dropDisk();
  }
  
  else if(currentPosition > remoteColumn){
    moveDistance = currentPosition-remoteColumn;
    anticlockwise(moveDistance);
    dropDisk();
  }

  else{
    Serial.println("wrong remoteColumn");
  }

  currentPosition=remoteColumn;  //update current position
}

void clockwise(int moveDistance){
  //move in the clockwise direction
  for(int i = 0; i < moveDistance*380; i++){
    for(int j = 7; j >= 0; j--){
      setDriverIn(j);
      delay(1);
    }
  }
}

void anticlockwise(int moveDistance){
  //move in the anticlockwise direction
  for(int i = 0; i < moveDistance*380; i++){
    for(int j = 0; j < 8; j++){
     setDriverIn(j);
     delay(1);
    }
  }
}


void setDriverIn(int i){
  //send step sequence to stepper
  digitalWrite(2, bitRead(stepSequance[i], 0));
  digitalWrite(3, bitRead(stepSequance[i], 1));
  digitalWrite(5, bitRead(stepSequance[i], 2));
  digitalWrite(6, bitRead(stepSequance[i], 3));

}

void dropDisk(){
  //drop disck areturn to initial position
  servoDrop.write(180);   // turn servo to 90 degrees
  delay(500);            // wait half a second
  servoDrop.write(90);    // return servo to initial position
  delay(500);            // wait half a second

}
