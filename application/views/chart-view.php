<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>

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
        <div class=" container col-lg-6 mt-3" style="margin-top: 20px;">
            <div class="p-3 border bg-light">
                <button class="btn btn-info " id="save" style="position: absolute; top: 2px; right: 15px"><i class="fa fa-download"></i></button>
                <canvas id="myChart3" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
            </div>
        </div>
        </div>
    </div>
</div>

<script>

var chartDataAirtel = <?= json_encode($chart_data) ?>;

const dataAirtel = {
  labels: [
    'Airtel',
    'BOA',
  ],
  datasets: [{
    data: [chartDataAirtel["airtel"]["solde"]["solde_airtel"] , chartDataAirtel["airtel"]["solde"]["solde_boa"]],
    backgroundColor: [
      'rgb(255, 99, 132)',
      'rgb(54, 162, 235)',
    ],
    hoverOffset: 4
  }]
};

const configAirtel = {
  type: 'doughnut',
  data: dataAirtel,
};

var chartDataTelma = <?= json_encode($chart_data) ?>;

console.log(chartDataTelma);
const dataTelma = {
  labels: [
    'Telma',
    'BOA',
  ],
  datasets: [{
    data: [chartDataTelma["telma"]["solde"]["solde_telma"] , chartDataTelma["telma"]["solde"]["solde_boa"]],
    backgroundColor: [
      'rgb(75, 192, 192)',
      'rgb(54, 162, 235)',
    ],
    hoverOffset: 4
  }]
};

const configTelma = {
  type: 'doughnut',
  data: dataTelma,
};


// var chartDataOrange = <?= json_encode($chart_data) ?>;
const dataOrange = {
  labels: [
    'Orange',
    'BOA',
  ],
  datasets: [{
    data: [1 , 1],
    backgroundColor: [
      'rgb(255, 165, 0)',
      'rgb(54, 162, 235)',
    ],
    hoverOffset: 4
  }]
};

const configOrange = {
  type: 'doughnut',
  data: dataOrange,
};


let ctx = document.getElementById('myChart').getContext('2d');

let myChart = new Chart(ctx, configAirtel);

let ctx2 = document.getElementById('myChart2').getContext('2d');

let myChart2 = new Chart(ctx2, configTelma);

let ctx3= document.getElementById('myChart3').getContext('2d');

let myChart3 = new Chart(ctx3, configOrange);


// var chartDataOrange = <?= json_encode($chart_data) ?>;
// var ctx = document.getElementById('myChart2').getContext('2d');


// var myChart = new Chart(ctx, {
//     type: 'doughnut',
//     data: chartDataOrange,
//     options: {
//         // Options du graphique
//     }
// });

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


