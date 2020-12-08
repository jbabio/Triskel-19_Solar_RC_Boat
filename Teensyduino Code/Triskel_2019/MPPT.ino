const char MPPT_PO_HEADER[] = "*** START OF MPPT_PO ***";
const char MPPT_PO_FOOTER[] = "***  END OF MPPT_PO  ***";

const int READS_PER_SAMPLE  = 10;
const int THROTTLE_STEP     = 1;
const int MIN_THROTTLE      = 1000;
const int MAX_THROTTLE      = 2000;
const int THROTTLE_THR      = 1500;


float mean_amps   = 0;
float mean_volts  = 0;
float mean_power  = 0;
float last_amps   = 0;
float last_volts  = 0;
float last_power  = 0;

int last_throttle = 0;

int read_count    = 0;


void MPPT_PO()
  {
  Serial.println(MPPT_PO_HEADER);

  int throttle = map(channels[1], 172, 1811, 1000, 2000);     // scaled to send a pulse between 1us & 2us

  //Activate PO mode if received throttle is over threshold.
  if ( throttle > THROTTLE_THR )
    {
    Serial.println("THROTTLE IS OVER THRESHOLD - MPPT P&O ACTIVE.");

    //Mean value for volts and amps to get a stable reading.
    if ( read_count < READS_PER_SAMPLE )
      {
      read_count++;
      mean_volts += volts / READS_PER_SAMPLE ;
      mean_amps += amps / READS_PER_SAMPLE ;
      Serial.print("Volts & Amps Sample #");
      Serial.println(read_count);
      Serial.print("volts: ");
      Serial.println(volts);
      Serial.print("amps: ");
      Serial.println(amps);
      Serial.print("power: ");
      Serial.println(volts*amps);
      }
    else
      {
      read_count = 0;

      mean_power = mean_volts * mean_amps ;

      Serial.print("mean_amps: ");
      Serial.println(mean_amps);
      Serial.print("mean_volts: ");
      Serial.println(mean_volts);
      Serial.print("mean_power: ");
      Serial.println(mean_power);
      Serial.print("last_amps: ");
      Serial.println(last_amps);
      Serial.print("last_volts: ");
      Serial.println(last_volts);
      Serial.print("last_power: ");
      Serial.println(last_power);
      Serial.print("throttle: ");
      Serial.println(throttle);

  
      //If in PO mode, received throttle is useless. Use last_throttle instead.
      throttle = last_throttle;
      
      if (mean_power > last_power)
        {
        if (mean_volts > last_volts)  { throttle -=  THROTTLE_STEP; Serial.println("Climbing from left side, decrease throttle."); }
        else                          { throttle +=  THROTTLE_STEP; Serial.println("Climbing from right side, increase throttle."); }
        }
      else
        {
        if (mean_volts > last_volts)  { throttle +=  THROTTLE_STEP; Serial.println("Descending towards right side, increase throttle."); }
        else                          { throttle -=  THROTTLE_STEP; Serial.println("Descending towards left side, decrease throttle."); }
        }

      throttle = constrain(throttle, MIN_THROTTLE, MAX_THROTTLE);
      Serial.print("new throttle: ");
      Serial.println(throttle);

      //Save volts, amps and throttle for next iteration.
      last_volts = mean_volts;
      last_amps = mean_amps;
      last_power = last_volts * last_amps;
      last_throttle = throttle;

      mean_volts = 0;
      mean_amps = 0;

      }
    }
  else
    {
    read_count = 0;

    Serial.println("THROTTLE IS UNDER THRESHOLD - MPPT DEACTIVATED.");
    Serial.print("volts: ");
    Serial.println(volts);
    Serial.print("amps: ");
    Serial.println(amps);
    Serial.print("power: ");
    Serial.println(volts*amps);
    Serial.print("throttle: ");
    Serial.println(throttle);

    throttle = constrain(throttle, MIN_THROTTLE, MAX_THROTTLE);
    Serial.print("new throttle: ");
    Serial.println(throttle);
    }
    

  rcservoout.writeMicroseconds(throttle);
  Serial.println(MPPT_PO_FOOTER);
  }
