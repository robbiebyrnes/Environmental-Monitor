<!doctype html public "-//w3c//dtd html 3.2//en">
<html>
<head>
<title>Derriston Weather Station: Multi Variable Line Chart</title>
</head>
<body >
<?Php
require "config.php";// Database connection

if($stmt = $connection->query("SELECT reading_time, Temperature, Humidity ,WindSpeed, ID FROM SensorData WHERE reading_time >= now() - INTERVAL 1 MONTH ORDER BY reading_time")){
$php_data_array = Array(); // create PHP array
while ($row = $stmt->fetch_row()) {
   $php_data_array[] = $row; // Adding to array
   }

}else{
echo $connection->error;
}
//print_r( $php_data_array);
// You can display the json_encode output here. 
// echo json_encode($php_data_array); 

// Transfor PHP array to JavaScript two dimensional array 
echo "<script>
        var Multi_Var = ".json_encode($php_data_array)."
</script>";
?>

<center><div id="curve_chart"></div></center>
<br><br>
<center><a href=https://www.envirosample.online/derriston/charts/line-chart-24.php>View latest 24 Hours</a></center>
<center><a href=https://www.envirosample.online/derriston/view-tabular-data.php>View Current Readings Table</a></center>

<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
<script type="text/javascript">

      // Load the Visualization API and the corechart package.
      google.charts.load('current', {packages: ['corechart']});
      google.charts.setOnLoadCallback(drawChart);
	  
      function drawChart() {

        // Create the data table.
        var data = new google.visualization.DataTable();
        data.addColumn('string', 'reading_time');
        data.addColumn('number', 'Temperature');
        data.addColumn('number', 'Humidity');
		data.addColumn('number', 'WindSpeed');
        for(i = 0; i < Multi_Var.length; i++)
        data.addRow([Multi_Var[i][0],parseInt(Multi_Var[i][1]),parseInt(Multi_Var[i][2]),parseInt(Multi_Var[i][3])]);
     
        var options = {
        title: 'Derriston Weather Station - Temperature, Humidity & Wind Speed - latest MONTH',
        //titleTextStyle: {
        //color: <string>,    // any HTML string color ('red', '#cc00cc')
        //fontName: <string>, // i.e. 'Times New Roman'
        //fontSize: 30, // 12, 18 whatever you want (don't specify px)
        //: true,    // true or false
        //italic: <boolean>   // true of false
        //}
        curveType: 'function',
		width: 1000,
        height: 500,
        legend: { position: 'bottom' }
        };
        hAxis : { 
            textStyle : {
                fontSize: 3
             }
        };

        var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));
        chart.draw(data, options);
       }
	///////////////////////////////
</script>
</body></html>


