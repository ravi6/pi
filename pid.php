<?php
/* Author: Ravi Saripalli 
 Both of these will be null when trying to setup PWM pin
 ini_set('display_errors', 1);
 ini_set('display_startup_errors', 1);
 error_reporting(E_ALL);
 NB: We make all shell_exec commands non blocking
      by spawing all shell commands to background process
      and discard any error or terminal outputs from the 
      shell command
*/

$cmd = $_POST['cmd'];
$Kc = $_POST['Kc'];
$Ki = $_POST['Ki'];
$Kd = $_POST['Kd'];

if ($cmd == 0) {
  // Remove run flag file
    shell_exec("/bin/rm  run >& /dev/null &") ;
    echo "pid Control Stopped" ;
} else if ($cmd == 1) { 
    // Clobber existing pidData contents
    $fp = fopen("pidData", 'w');
    fclose($fp);
    // Create a run flag file
    shell_exec("touch run >& /dev/null &") ;

    // run pid in background so that php is nonblocking
    shell_exec("./pid > /dev/null  &");
    echo "pid Control Started"  ;  // return status
} else if ($cmd == 2){  // Updates pidParams
    $msg =   $Kc ." ". $Ki ." ". $Kd  ;
    file_put_contents("pidParams", $msg, LOCK_EX);
    $out = "pidParams updated: " . $msg ;
    echo $out ;  // return status
} else if($cmd == 3) {  // generate PID plot
    $shcmd = "gnuplot pidPlot >& /dev/null &" ;
    shell_exec($shcmd); 
    echo "gnuplot updated: "  ;
} else
    echo "Don't understand command code " . $cmd ;
?>  
