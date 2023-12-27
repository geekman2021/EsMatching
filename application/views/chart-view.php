<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>

<?php
$soldeAirtel = isset($chart_data["airtel"]["solde"]->solde_airtel) ? $chart_data["airtel"]["solde"]->solde_airtel : 0;
$soldeOrange = isset($chart_data["orange"]["solde"]->solde_orange) ? $chart_data["orange"]["solde"]->solde_orange : 0;
$soldetelma = isset($chart_data["telma"]["solde"]->solde_telma) ? $chart_data["telma"]["solde"]->solde_telma : 0;
$princ_solde = isset($chart_data["orange"]["solde"]->princ_solde) ? $chart_data["orange"]["solde"]->princ_solde : 0;
$comm_solde = isset($chart_data["orange"]["solde"]->comm_solde) ? $chart_data["orange"]["solde"]->comm_solde : 0;

$solde_boaAirtel= isset($chart_data["airtel"]["solde"]->solde_boa) ? $chart_data["airtel"]["solde"]->solde_boa : 0;
$solde_boaTelma= isset($chart_data["telma"]["solde"]->solde_boa) ? $chart_data["telma"]["solde"]->solde_boa : 0;
$solde_boaOrange= isset($chart_data["orange"]["solde"]->orange_solde) ? $chart_data["orange"]["solde"]->orange_solde : 0;
?>

<div class="container-fluid">
    <div class="row" >
        <div class=" mt-3 col-lg-6" >
            <div class="p-3 border bg-light ">
				      <button class="btn btn-info " id="save" style="position: absolute; top: 2px; right: 15px"><i class="fa fa-download"></i></button>
              <canvas id="myChart" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
              <div class="d-flex align-items-center justify-content-between">
                <h4>Solde Airtel: </h4><?= number_format($soldeAirtel, 0 , ',', ' ') ?> Ar
                <h4>Solde BOA: </h4><?= number_format($solde_boaAirtel, 0, ',', ' ')  ?> Ar
              </div>
              
            </div>
        </div>
        <div class="col-lg-6 mt-3">
            <div class="p-3 border bg-light">
                <button class="btn btn-info " id="save" style="position: absolute; top: 2px; right: 15px"><i class="fa fa-download"></i></button>
                <canvas id="myChart2" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
                <div class="d-flex align-items-center justify-content-between">
                  <h4>Solde Telma: </h4><?= number_format(-($soldetelma), 0, ',', ' ') ?> Ar
                  <h4>Solde BOA: </h4><?= number_format($solde_boaTelma, 0, ',', ' ') ?> Ar
                </div>
                  
            </div> 
        </div> 
        <div class=" container col-lg-8 mt-3" style="margin-top: 20px;">
            <div class="p-3 border bg-light">
                <button class="btn btn-info " id="save" style="position: absolute; top: 2px; right: 15px"><i class="fa fa-download"></i></button>
                <canvas id="myChart3" style="background-color:rgba(185, 240, 231, 0.5);"></canvas>
                <div class="d-flex align-items-center justify-content-between">
                  <h4>Solde Comm: </h4> <?= number_format(-($comm_solde), 0, ',', ' ') ?> Ar
                  <h4>Solde Princ: </h4> <?= number_format(-($princ_solde), 0, ',', ' ') ?> Ar
                  <h4>Solde Orange: </h4> <?= number_format($solde_boaOrange, 0, ',', ' ') ?> Ar
                </div>
                
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


var chartDataOrange = <?= json_encode($chart_data) ?>;

console.log(chartDataOrange);

const dataOrange = {
  labels: [
    'commission',
    'principal',
    'orange'
  ],
  datasets: [{
    data: [chartDataOrange["orange"]["solde"]["comm_solde"], chartDataOrange["orange"]["solde"]["princ_solde"], chartDataOrange["orange"]["solde"]["orange_solde"]], 
    backgroundColor: [
      'rgb(255, 102, 102)',
      'rgb(54, 162, 235)',
      'rgb(255, 165, 0)',
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


