<?php
// Author: Ravi Saripalli (29th Aug. 2021)
// Process the Json data from POST ajax call
// which proivdes  Frequency, and Duty Cycle %
// Run my shell script that sets up the
// Hardware based Pulse Generator on the Onion
//
// Nb.  If the JSON data in POST message is null
//     we assume that we are trying to setup PWM
//     In this instance pulse script does not need
//     any inputs
// error_reporting(E_ALL); --> debugging

// Both of these will be null when trying to setup PWM pin
$Hz = $_POST['Hz'];
$Duty = $_POST['Duty'];
$cmd =  "/var/www/html/pulse/pulse " ;

if ( is_null($Hz) ) { // just PWM  setup
  $out =  shell_exec($cmd . " 2>&1");
  echo $out ;  // return status
} else { // Setting signal pattern
    $out = shell_exec($cmd . $Hz . " " . $Duty . " 2>&1");
    echo $out ;  // Confirmation of successful freq, duty setup
}
?>  
