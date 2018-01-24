<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<title>Temp and Humidity</title>

	<!-- 1. Add these JavaScript inclusions in the head of your page -->
	<script type="text/javascript" src="http://code.jquery.com/jquery-1.10.1.js"></script>
	<script type="text/javascript" src="http://code.highcharts.com/highcharts.js"></script>
        <script src="https://code.highcharts.com/js/modules/exporting.js"></script>
	<!-- Container for Chart B -->
	<!-- 2. Add the JavaScript to initialize the chart on document ready -->
<script>
$(function() {
	$('#chart-A').highcharts({
		chart: {
			type: 'spline',
			zoomType: 'xy',
                        backgroundColor: '#fff'
		},
		title: {
			text: 'Recent Temperature (C)'
		},
		xAxis: {
			type: 'value',
			tickPixelInterval: 150,
			maxZoom: 10
		},
		yAxis: {
			minPadding: 0.2,
			maxPadding: 0.2,
			title: {
				text: 'Celsius',
				margin: 1
			}
		},
		legend: {
		    enabled: false
		},
	    series: [{
	        name: 'Deg C',
                color: '#ff0000',
                lineWidth: 5,
	        data: [
<?php
// Open the CSV maintained by the background Python script and load
// our array of data with the information stored in there (since midnight)
if (($handle = fopen("DHT22_temperatures.csv", "r")) !== FALSE) {
	while (($data = fgetcsv($handle, 1000, ",")) !== FALSE) {
		if (count($data) >= 2) { // valid lines have 2 or more columns of data
			echo "[" . $data[0] . "," . $data[3] . "],\n";
		}
	}
	fclose($handle);
}
?>
	            ]
	        }]
	});
	
	$('#chart-B').highcharts({
		chart: {
			type: 'spline',
			zoomType: 'xy',
                        backgroundColor: '#fff'
		},
		title: {
			text: 'Recent Humidity'
		},
		xAxis: {
			type: 'value',
			tickPixelInterval: 150,
			maxZoom: 10
		},
		yAxis: {
			minPadding: 0.2,
			maxPadding: 0.2,
			title: {
				text: '%',
				margin: 1
			}
		},
		legend: {
		    enabled: false
		},
	    series: [{
	        name: 'Hum %',
                color: '#ff0000',
                lineWidth: 5,
	        data: [
<?php
// Open the CSV maintained by the background Python script and load
// our array of data with the information stored in there (since midnight)
if (($handle = fopen("DHT22_humidities.csv", "r")) !== FALSE) {
	while (($data = fgetcsv($handle, 1000, ",")) !== FALSE) {
		if (count($data) >= 2) { // valid lines have 2 or more columns of data
			echo "[" . $data[0] . "," . $data[3] . "],\n";
		}
	}
	fclose($handle);
}
?>
	            ]
	        }]
	    });
    });

</script>

</head>
<body>
	<div id="chart-A" class="chart"></div>
	<!-- Container for Chart A -->
	<form method="get" action="DHT22_temperatures.csv">
	   <button type="submit">Temperature CSV</button>
	</form>
	<div class="spacer"></div>
	<div id="chart-B" class="chart"></div>
	<form method="get" action="DHT22_humidities.csv">
	   <button type="submit">Humidity CSV</button>
	</form>
</body>
</html>