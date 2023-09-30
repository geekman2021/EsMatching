<div class="container-fluid">
    <div class="row" >
        <div class=" mt-3 col-lg-6" >
            <div class="p-3 border bg-light">
				<button class="btn btn-info " id="save" style="position: absolute; top: 2px; right: 15px"><i class="fa fa-download"></i></button>
                <canvas id="myChart" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
				
            </div>
        </div>
        <div class="col-lg-6 mt-3">
            <div class="p-3 border bg-light">
                <button class="btn btn-info " id="save" style="position: absolute; top: 2px; right: 15px"><i class="fa fa-download"></i></button>
                <canvas id="myChart2" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
            </div> 
        </div> 
        <div class="col-lg-12 mt-3" style="margin-top: 20px;">
            <div class="p-3 border bg-light">
                <button class="btn btn-info " id="save" style="position: absolute; top: 2px; right: 15px"><i class="fa fa-download"></i></button>
                <canvas id="myChart3" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
            </div>
        </div>
        </div>
    </div>
</div>

<script>
const data = {
  labels: [
    'Airtel',
    'BOA',
  ],
  datasets: [{
    label: 'My First Dataset',
    data: [300, 50],
    backgroundColor: [
      'rgb(255, 99, 132)',
      'rgb(54, 162, 235)',
    ],
    hoverOffset: 4
  }]
};

const config = {
  type: 'doughnut',
  data: data,
};


var chartDataAirtel = <?= json_encode($chart_data) ?>;
var ctx = document.getElementById('myChart').getContext('2d');


var myChart = new Chart(ctx, config);

var chartDataOrange = <?= json_encode($chart_data) ?>;
var ctx = document.getElementById('myChart2').getContext('2d');


var myChart = new Chart(ctx, {
    type: 'doughnut',
    data: chartDataOrange,
    options: {
        // Options du graphique
    }
});

// var chartDataTelma = <?= json_encode($chart_data) ?>;
// var ctx = document.getElementById('myChart3').getContext('2d');

// var myChart = new Chart(ctx, {
//     type: 'bar',
//     data: chartDataTelma,
//     options: {
//         // Options du graphique
//     }
// });
</script>


