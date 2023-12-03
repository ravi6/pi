<?php
// Author: Ravi Saripalli 
// Both of these will be null when trying to setup PWM pin
$cmd = $_POST['cmd'];
$sp = $_POST['sp'];
$Kc = $_POST['Kc'];
$Ki = $_POST['Ki'];
$Kd = $_POST['Kd'];
$shcmd =  "/home/ravi/learn/pi/pid" ;

if ($cmd == 1) { 
 $out =  shell_exec($shcmd . " 2>&1");
 $out = "hello" ; 
 echo "pid control Started : Shell RC = " . $out ;  // return status
} else if ($cmd == 0){  // stop pid control
    $msg = $cmd ." ". $sp  ." ". $Kc ." ". $Ki ." ". $Kd  ;
    $fp = fopen("pidParams", 'w');
       flock($fp, LOCK_EX) ;
       fwrite($fp, $msg) ;    // update pidParams
       flock($fp, LOCK_UN);
       fclose($fp) ;
     $out = "pidParams updated: " . $msg ;
     echo $out ;  // return status
} else if($cmd == 2) {  // generate PID plot
    $shcmd = "gnuplot pidPlot" ;
    $rc = shell_exec($shcmd . " 2>&1"); 
    echo "plot updated" ;
} else
    echo "Don't understand command code " . $cmd ;
?>  
