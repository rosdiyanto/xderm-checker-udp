<?php
exec("echo Stop > /www/xderm/log/st;curl -s http://localhost/xderm/index.php -d 'button1=Stop'",$out1 , $statusStop);
sleep(3);
exec("echo Start > /www/xderm/log/st;curl -s http://localhost/xderm/index.php -d 'button1=Start'",$out2 , $statusStart);

if ($statusStop == 0 && $statusStart == 0) {
     echo "Sukses";
 } else {
     echo "Gagal";
}
?>