<?php
	// save configuration
    if(isset($_POST["config"])){
        if(file_put_contents("config.txt", $_POST["config"])){
            //echo "configuration saved!";
        }

		//echo "<pre>".$_POST["config"]."</pre>";
		header("Location: index.php");
    }
?>
<?php
	// capture new image
	if(isset($_GET["capture"])){
		shell_exec("python captureImage.py");
		header("Location: index.php");
	}
?>
<?php
	// load config
	$config = file_get_contents("config.txt");
	$config = str_replace("\n", " ", $config);
	$values = explode(" ", $config);
	if(count($values) <= 1){
		$values = array(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
	}
?>
<head>
	<meta charset="utf-8">
	<style>
		body{
			margin:5px;
			font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
		}

		.panes{
			display: flex;
			flex-wrap: wrap;
		}

		.pane{
			border: 1px solid #333;
			border-radius: 3px;
			padding: 10px;
			margin: 10px;
		}

		.pane .title{
			position: relative;
			top: -30px;
			margin-bottom: -30px;
			background: #fff;
			display: inline-block;
			padding: 5px;
		}

		.pane canvas, .pane img, .pane textarea{
			display: block;
		}

		.histogram{
			border: 1px solid #aaa;
		}

		.color{
			width: 200px;
		}

		.config{
			width: 300px;
			height: 200px;
		}

		.colorBox{
			width: 50px;
			height: 50px;
			border: 1px solid black;
			margin: 10px 10px 10px 0;
			display: inline-block;
		}
	</style>
</head>
<body>
	<h1><img src="wizzard.jpg" height="64">Connect4 Bot Configuration Wizzard</h1>
	<div class="panes">
		<div class="pane">
			<div class="title">Analysed image</div>
			<canvas id="canvas"></canvas>
		</div>
		<div class="pane">
			<div class="title">Original image</div>
			<img src="capture.png?t=<?php echo time();?>">
		</div>
		<div class="pane">
			<div class="title">Histograms</div>
			<div>Hue histogram</div>
			<canvas id="hueHistogram" class="histogram"></canvas>
			<div>Saturation histogram</div>
			<canvas id="saturationHistogram" class="histogram"></canvas>		
		</div>
		<div class="pane">
				<div class="title">Settings</div>
				<br>
				<label>Cell size </label><input id="cellSize" type="number" onchange="updateSettings()" min="1" max="100"><br>
				<label>Saturation threshold </label><input id="saturationThreshold" type="number" onchange="updateSettings()" min="0" max="255"><br>
				<label>PX count threshold </label><input id="pxCountThreshold" type="number" step="0.001" onchange="updateSettings()" min="0" max="1"><br>
				<a href="index.php?capture"><button>Capture test image</button></a>
				<button onclick="resetPoints()">Reset points</button>
			</div>
		<div class="pane">
			<div class="title">Color picker</div>
			<div class="color" id="color">Click on analysed image to get RGB and HSV color information about the pixel</div>
			<div class="colorBox" id="colorBox"></div>
			<div class="colorBox" id="colorBoxHue"></div>
		</div>
		<div class="pane">
			<div class="title">Compiled configuration</div>
			<form method="POST" action="index.php">
				<textarea name="config" id="config" class="config" readonly></textarea>
				<input type="submit">
			<form>
		</div>
	</div>
	
<script>
	let c = document.getElementById("canvas");
	let ctx = c.getContext("2d");

	let hueHistogramCanvas = document.getElementById("hueHistogram");
	let hueHistogramCtx = hueHistogramCanvas.getContext("2d");

	let saturationHistogramCanvas = document.getElementById("saturationHistogram");
	let saturationHistogramCtx = saturationHistogramCanvas.getContext("2d");

	let size = <?php echo $values[13]; ?>;

	c.width = 1024;
	c.height = 768;

	let hueThreshold = {
		X: {
			min: <?php echo $values[8]; ?>,
			max: <?php echo $values[9]; ?>,
			minGrab: false,
			maxGrab: false
		},
		O: {
			min: <?php echo $values[10]; ?>,
			max: <?php echo $values[11]; ?>,
			minGrab: false,
			maxGrab: false
		}
	}

	let colorX = "#fff600";
	let colorO = "#ff7200";
	let colorXlight = "#fff842";
	let colorOlight = "#ff8e32";

	let rgbXlight = hexToRgb(colorXlight);
	let rgbOlight = hexToRgb(colorOlight);
	let rgbX = hexToRgb(colorX);
	let rgbO = hexToRgb(colorO);

	let saturationThreshold = <?php echo $values[12]; ?>;
	let threshold = <?php echo $values[14]; ?>;
	let points = [
		{x: <?php echo $values[0]; ?>, y: <?php echo $values[1]; ?>, grab: false, name: "zgoraj-desno"},
		{x: <?php echo $values[2]; ?>, y: <?php echo $values[3]; ?>, grab: false, name: "spodaj-desno"},
		{x: <?php echo $values[4]; ?>, y: <?php echo $values[5]; ?>, grab: false, name: "spodaj-levo"},
		{x: <?php echo $values[6]; ?>, y: <?php echo $values[7]; ?>, grab: false, name: "zgoraj-levo"},
	];
	bg = new Image();
	bg.onload = render;
	bg.src = "capture.png?t=<?php echo time();?>";

	function render(){
		c.width = bg.width;
		c.height = bg.height;

		hueHistogram = new Array(256).fill(0);
		saturationHistogram = new Array(256).fill(0);

		ctx.clearRect(0,0,c.width, c.height);

		// izrisi osnovno sliko
		ctx.drawImage(bg,0,0);

		// pojdi cez vsa polja in jih analiziraj
		let w = 7-1;
		let h = 6-1;
		for(let i=0; i<h+1; i++){
			let x1 = (points[0].x*(h-i) + points[1].x*(i))/h;
			let y1 = (points[0].y*(h-i) + points[1].y*(i))/h;

			let x2 = (points[3].x*(h-i) + points[2].x*(i))/h;
			let y2 = (points[3].y*(h-i) + points[2].y*(i))/h;

			for(let j=0; j<w+1; j++){
				let x = (x1*(w-j) + x2*j)/w;
				let y = (y1*(w-j) + y2*j)/w;
				
				let nX = 0;
				let nO = 0;
				let n = 0;
				let imd = ctx.getImageData(x-size, y-size, size*2, size*2);
				
				// pojdi cez vse piksle v kvadratu in jih prestej
				
				for(let i=0; i<imd.data.length; i+=4){
					hsv = RGBtoHSV(imd.data[i+0], imd.data[i+1], imd.data[i+2]);
					hueHistogram[Math.round(hsv.h*255)]++;
					saturationHistogram[Math.round(hsv.s*255)]++;

					imd.data[i+0] = 200;
					imd.data[i+1] = 200;
					imd.data[i+2] = 200;
					if(hsv.s*255 > saturationThreshold){
						if(hueThreshold.X.min > hueThreshold.X.max){
							if(hueThreshold.X.min < hsv.h*255 || hsv.h*255 < hueThreshold.X.max){
								imd.data[i+0] = rgbXlight.r;
								imd.data[i+1] = rgbXlight.g;
								imd.data[i+2] = rgbXlight.b;
								nX++;
							}
						}
						else{
							if(hueThreshold.X.min < hsv.h*255 && hsv.h*255 < hueThreshold.X.max){
								imd.data[i+0] = rgbXlight.r;
								imd.data[i+1] = rgbXlight.g;
								imd.data[i+2] = rgbXlight.b;
								nX++;
							}
						}
						if(hueThreshold.O.min > hueThreshold.O.max){
							if(hueThreshold.O.min < hsv.h*255 || hsv.h*255 < hueThreshold.O.max){
								imd.data[i+0] = rgbOlight.r;
								imd.data[i+1] = rgbOlight.g;
								imd.data[i+2] = rgbOlight.b;
								nO++;
							}
						}
						else{
							if(hueThreshold.O.min < hsv.h*255 && hsv.h*255 < hueThreshold.O.max){
								imd.data[i+0] = rgbOlight.r;
								imd.data[i+1] = rgbOlight.g;
								imd.data[i+2] = rgbOlight.b;
								nO++;
							}
						}
					}
					else{
						imd.data[i+0] = 255;
						imd.data[i+1] = 255;
						imd.data[i+2] = 255;
					}
					n++;
				}

				ctx.putImageData(imd, x-size, y-size);

				// oznaci poje
				ctx.beginPath();
				ctx.arc(x, y, 5, 0, 2 * Math.PI, false);
				if(nX > n*threshold)ctx.fillStyle = colorX;
				else if(nO > n*threshold)ctx.fillStyle = colorO;
				else ctx.fillStyle = '#000';
				ctx.strokeStyle = "#000";
				ctx.fill();
				ctx.stroke();
			}
		}

		// izrisi rocaje za premikanje kotov igralne plosce
		for(let p of points){
			ctx.beginPath();
			ctx.arc(p.x, p.y, 10, 0, 2 * Math.PI, false);
			ctx.lineWidth = 2;
			ctx.strokeStyle = '#ef1cff';
			ctx.stroke();
		}


		// izpisi zgenerirano konfiguracijsko datoteko
		let config = "";
		for(let p of points){
			config += Math.round(p.x)+' '+ Math.round(p.y) +'\n';
		}
		config += Math.round(hueThreshold.X.min) + " " + Math.round(hueThreshold.X.max) + "\n";
		config += Math.round(hueThreshold.O.min) + " " + Math.round(hueThreshold.O.max) + "\n";
		config += Math.round(saturationThreshold) + "\n";
		config += Math.round(size) + "\n";
		config += threshold + "\n";
		document.getElementById("config").innerHTML = config;


		// render hue histogram
		renderHueHitorgram(hueHistogram);

		// render saturation histogram
		renderSaturationHistogram(saturationHistogram);

		renderSettings();
	}

	function renderHueHitorgram(hueHistogram){
		let maxHeigth = Math.max(...hueHistogram);
		let height = 200;
		let width = hueHistogram.length*2
		let yscale = maxHeigth/height;
		hueHistogramCanvas.width = width;
		hueHistogramCanvas.height = height;
		for(let i=0; i<hueHistogram.length; i++){
			let rgb = HSVtoRGB(i/255,1,1);
			hueHistogramCtx.fillStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
			hueHistogramCtx.fillRect(i*2,height-hueHistogram[i]/yscale,2,hueHistogram[i]/yscale);
		}

		hueHistogramCtx.fillStyle = colorX;
		hueHistogramCtx.strokeStyle = colorX;
		if(hueThreshold.X.min > hueThreshold.X.max){
			hueHistogramCtx.fillRect(hueThreshold.X.min*2, 0, width-hueThreshold.X.min*2, 10);
			hueHistogramCtx.fillRect(0, 0, hueThreshold.X.max*2, 10);
			hueHistogramCtx.strokeRect(hueThreshold.X.min*2, -1, (hueThreshold.X.max-hueThreshold.X.min)*2, height+2);
		}
		else{
			hueHistogramCtx.fillRect(hueThreshold.X.min*2, 0, (hueThreshold.X.max-hueThreshold.X.min)*2, 10);
			hueHistogramCtx.strokeRect(hueThreshold.X.min*2, 0, (hueThreshold.X.max-hueThreshold.X.min)*2, height+1);
		}


		hueHistogramCtx.fillStyle = colorO;
		hueHistogramCtx.strokeStyle = colorO;
		if(hueThreshold.O.min > hueThreshold.O.max){
			hueHistogramCtx.fillRect(hueThreshold.O.min*2, 0, width-hueThreshold.O.min*2, 10);
			hueHistogramCtx.fillRect(0, 0, hueThreshold.O.max*2, 10);
			hueHistogramCtx.strokeRect(hueThreshold.O.min*2, -1, (hueThreshold.O.max-hueThreshold.O.min)*2, height+2);
		}
		else{
			hueHistogramCtx.fillRect(hueThreshold.O.min*2, 0, (hueThreshold.O.max-hueThreshold.O.min)*2, 10);
			hueHistogramCtx.strokeRect(hueThreshold.O.min*2, 0, (hueThreshold.O.max-hueThreshold.O.min)*2, height+1);
		}


	}

	function renderSaturationHistogram(saturatioinHistogram){
		saturationHistogramCanvas.width = saturationHistogram.length*2;
		let maxHeigth = Math.max(...saturationHistogram);
		let height = 200;
		let yscale = maxHeigth/height;
		saturationHistogramCanvas.height = height;
		for(let i=0; i<saturationHistogram.length; i++){
			let rgb = HSVtoRGB(0,i/255,i/255);
			saturationHistogramCtx.fillStyle = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;
			saturationHistogramCtx.fillRect(i*2,height-saturationHistogram[i]/yscale,2,saturationHistogram[i]/yscale);
		}
	}

	function renderSettings(){
		document.getElementById("saturationThreshold").value = saturationThreshold;
		document.getElementById("cellSize").value = size;
		document.getElementById("pxCountThreshold").value = threshold;
	}

	function updateSettings(){
		saturationThreshold = document.getElementById("saturationThreshold").value;
		size = document.getElementById("cellSize").value;
		threshold = document.getElementById("pxCountThreshold").value;
		render();
	}

	function resetPoints(){
		points = [
			{x: 120, y: 20, grab: false, name: "zgoraj-desno"},
			{x: 120, y: 120, grab: false, name: "spodaj-desno"},
			{x: 20, y: 120, grab: false, name: "spodaj-levo"},
			{x: 20, y: 20, grab: false, name: "zgoraj-levo"},
		];

		render();
	}

	function getPixelRGB(x,y){
		ctx.drawImage(bg,0,0);
		let data = ctx.getImageData(x,y,1,1).data;
		render();
		return {r: data[0], g: data[1], b: data[2]};
	}


	// main canvas eventlisteners

	c.addEventListener("click", function(e){

		let rect = e.target.getBoundingClientRect();
		let mouseX = e.clientX - rect.left;
		let mouseY = e.clientY - rect.top;

		let rgb = getPixelRGB(mouseX, mouseY);
		let hsv = RGBtoHSV(rgb);
		document.getElementById("color").innerHTML = `<pre>rgb(${rgb.r}, ${rgb.g}, ${rgb.b})\nhsv(${Math.round(hsv.h*255)}, ${Math.round(hsv.s*255)}, ${Math.round(hsv.v*255)})</pre>`;

		document.getElementById("colorBox").style.background = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;

		hsv.s = 1;
		hsv.v = 1;


		rgb = HSVtoRGB(hsv);
		console.log(hsv, rgb);

		document.getElementById("colorBoxHue").style.background = `rgb(${rgb.r}, ${rgb.g}, ${rgb.b})`;

	});


	c.addEventListener("mousedown", function(e){
		let rect = e.target.getBoundingClientRect();
		let mouseX = e.clientX - rect.left;
		let mouseY = e.clientY - rect.top;
		for(let p of points){
			let d = (p.x - mouseX)**2 + (p.y - mouseY)**2;
			if(d < 10**2){
				p.grabed = true;
				break;
			}
		}
	});

	c.addEventListener("mousemove", function(e){
		let rect = e.target.getBoundingClientRect();
		let mouseX = e.clientX - rect.left;
		let mouseY = e.clientY - rect.top;
		for(let p of points){
			if(p.grabed){
				p.x = mouseX;
				p.y = mouseY;
			}
		}

		render();
	});

	c.addEventListener("mouseup", function(e){
		for(let p of points){
			p.grabed = false;
		}
	});


	// hue histogram event listeners

	hueHistogramCanvas.addEventListener("mousedown", function(e){
		let rect = e.target.getBoundingClientRect();
		let mouseX = e.clientX - rect.left;
		let mouseY = e.clientY - rect.top;

		if(Math.abs(mouseX-hueThreshold.X.min*2) < 2){
			hueThreshold.X.minGrab = true;
		}
		else if(Math.abs(mouseX-hueThreshold.X.max*2) < 2){
			hueThreshold.X.maxGrab = true;
		}
		else if(Math.abs(mouseX-hueThreshold.O.min*2) < 2){
			hueThreshold.O.minGrab = true;
		}
		else if(Math.abs(mouseX-hueThreshold.O.max*2) < 2){
			hueThreshold.O.maxGrab = true;
		}
	});

	hueHistogramCanvas.addEventListener("mousemove", function(e){
		let rect = e.target.getBoundingClientRect();
		let mouseX = e.clientX - rect.left;
		let mouseY = e.clientY - rect.top;

		e.target.style.cursor = "default";
		if(Math.abs(mouseX-hueThreshold.X.min*2) < 2 || hueThreshold.X.minGrab){
			e.target.style.cursor = "e-resize";
		}
		if(Math.abs(mouseX-hueThreshold.X.max*2) < 2 || hueThreshold.X.maxGrab){
			e.target.style.cursor = "e-resize";
		}
		if(Math.abs(mouseX-hueThreshold.O.min*2) < 2 || hueThreshold.O.minGrab){
			e.target.style.cursor = "e-resize";
		}
		if(Math.abs(mouseX-hueThreshold.O.max*2) < 2 || hueThreshold.O.maxGrab){
			e.target.style.cursor = "e-resize";
		}

		if(hueThreshold.X.minGrab)hueThreshold.X.min = Math.round(mouseX/2);
		if(hueThreshold.X.maxGrab)hueThreshold.X.max = Math.round(mouseX/2);
		if(hueThreshold.O.minGrab)hueThreshold.O.min = Math.round(mouseX/2);
		if(hueThreshold.O.maxGrab)hueThreshold.O.max = Math.round(mouseX/2);

		render();
	});

	hueHistogramCanvas.addEventListener("mouseup", function(e){
		for(let p of points){
			p.grabed = false;
		}

		hueThreshold.X.minGrab = false;
		hueThreshold.X.maxGrab = false;
		hueThreshold.O.minGrab = false;
		hueThreshold.O.maxGrab = false;
		
	});



	// helper functions

	function RGBtoHSV(r, g, b) {
    if (arguments.length === 1) {
        g = r.g, b = r.b, r = r.r;
    }
    var max = Math.max(r, g, b), min = Math.min(r, g, b),
        d = max - min,
        h,
        s = (max === 0 ? 0 : d / max),
        v = max / 255;

    switch (max) {
        case min: h = 0; break;
        case r: h = (g - b) + d * (g < b ? 6: 0); h /= 6 * d; break;
        case g: h = (b - r) + d * 2; h /= 6 * d; break;
        case b: h = (r - g) + d * 4; h /= 6 * d; break;
    }

    return {
        h: h,
        s: s,
        v: v
    };
}

function HSVtoRGB(h, s, v) {
    var r, g, b, i, f, p, q, t;
    if (arguments.length === 1) {
        s = h.s, v = h.v, h = h.h;
    }
    i = Math.floor(h * 6);
    f = h * 6 - i;
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    return {
        r: Math.round(r * 255),
        g: Math.round(g * 255),
        b: Math.round(b * 255)
    };
}

function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

</script>
</body>
