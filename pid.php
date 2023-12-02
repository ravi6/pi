<?php
// Author: Ravi Saripalli 
// Both of these will be null when trying to setup PWM pin
$cmd = $_POST['cmd'];
$Kc = $_POST['Kc'];
$Ki = $_POST['Ki'];
$Kd = $_POST['Kd'];
$shcmd =  "/home/ravi/learn/pi/pid . $Kc  . $Ki . $Kd" ;

if (strcmp($cmd, "start") == 0) { 
  $out =  shell_exec($shcmd . " 2>&1");
  echo $out ;  // return status
} else {  // stop pid control
  $out = shell_exec("ls 2>&1") ;
  echo $out ;  // return status
}
?>  
