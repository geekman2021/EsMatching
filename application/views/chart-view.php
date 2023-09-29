<div class="container-fluid">
    <div class="row" >
        <div class="col-lg-6">
            <div class="p-3 mt-5 border bg-light">
				<button class="btn btn-info " id="save" style="position: absolute; top: 7px; right: 25px"><i class="fa fa-download"></i></button>
                <canvas id="myChart" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
				
            </div>
        </div>
        <div class="col-lg-6">
            <div class="p-3 mt-5 border bg-light">
				<button class="btn btn-info" id="save2" style="position:  absolute; top: 7px;right: 25px"><i class="fa fa-download"></i></button>
                <canvas id="myChart2" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
            </div> 
        </div> 
        <div class="col-lg-12" style="margin-top: 20px;">
            <div class="p-3 mt-5 border bg-light">
				<button class="btn btn-info" id="save3" style="position:  absolute; top: 7px;right: 25px"><i class="fa fa-download"></i></button>
                <canvas id="myChart3" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
            </div>
        </div>
        </div>
    </div>
</div>

<script>

var chartDataAirtel = <?= json_encode($chart_data) ?>;
var ctx = document.getElementById('myChart').getContext('2d');

var myChart = new Chart(ctx, {
    type: 'bar',
    data: chartDataAirtel,
    options: {
        // Options du graphique
    }
});

var chartDataOrange = <?= json_encode($chart_data) ?>;
var ctx = document.getElementById('myChart').getContext('2d');

var myChart = new Chart(ctx, {
    type: 'bar',
    data: chartDataOrange,
    options: {
        // Options du graphique
    }
});

var chartDataTelma = <?= json_encode($chart_data) ?>;
var ctx = document.getElementById('myChart').getContext('2d');

var myChart = new Chart(ctx, {
    type: 'bar',
    data: chartDataTelma,
    options: {
        // Options du graphique
    }
});



</script>


