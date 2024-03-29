<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>

<?php

$soldeAirtel = $chart_data["airtel"][0];
$soldetelma = $chart_data["telma"][0];
$solde_boaAirtel = $chart_data["airtel"][1];
$solde_boaTelma = $chart_data["telma"][1];
$princ_solde = $chart_data["orange"][0];
$comm_solde = $chart_data["orange"][1];
$soldeOrange = $chart_data["orange"][2];


$solde_ecart_telma= $chart_data["solde_ecart_telma"][0];
$solde_ecart_boa_telma= $chart_data["solde_ecart_telma"][1];
$solde_ecart_airtel= $chart_data["solde_ecart_airtel"][0];
$solde_ecart_boa_airtel= $chart_data["solde_ecart_airtel"][1];
$solde_ecart_boa_airtel= $chart_data["solde_ecart_airtel"][1];
$solde_ecart_orange= $chart_data["solde_ecart_orange"][0];
$solde_ecart_princ= $chart_data["solde_ecart_orange"][1];


// if(!empty($chart_data["ecart_telma"])) {
//   foreach($chart_data["ecart_telma"] as $item) {
//     $solde_ecart_telma += $item->solde;
//   }
// } if(!empty($chart_data["ecart_boa_telma"])){
//   foreach($chart_data["ecart_boa_telma"] as $item) {
//     $solde_ecart_boa_telma += $item->MONTANT;
//   }
// } if(!empty($chart_data["ecart_airtel"])) {
//   foreach($chart_data["ecart_airtel"] as $item) {
//     $solde_ecart_airtel+= $item->solde;
//   }
// } if(!empty($chart_data["ecart_boa_airtel"])) {
//   foreach($chart_data["ecart_boa_airtel"] as $item) {
//     $solde_ecart_boa_airtel+= $item->MONTANT;
//   }
// }

?>


    <!-- Cards Airtel -->
<div class="container-fluid mt-5">
<div class="row">
  <div class="col-lg-4 mb-4">
    <div class="card" style="border-radius:15px; border-left-color: #B80000; box-shadow: 0 2px 2px rgba(0, 0, 0, 0.50); border-width: 2px; ">
      <div class="card-header">
        <div class="row">
          <div class="col-lg-1">
            <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
          </div>
          <div class="col-lg-6">
            <h5 style="color: #312B81"><i class="bx" style="font-size: 22px; margin-left: -20px;"> Solde actuel </i></h5> 
          </div>
        </div>    
      </div>
      <div class="card-body">
        <div class="row">
            <div class="row py-2">
              <div class="col-lg-6">
                <img class="mb-1 mr-auto" src="<?= base_url()?>assets/images/airtel-logo.png" alt="airtel_logo" style="width: 75px; height: 20px;">
              </div>
              <div class="col-lg-6">
                <div class="text-xs font-weight-bold text-right text-uppercase mb-1" style="color: #00894A">BOA</div>
              </div>
              <div class="h5 mb-0 mt-2 font-weight-bold col-lg-6 mr-auto" style="color: #014D7F""><?= number_format($soldeAirtel, 0, ',' , ' '); ?> Ar</div>
              <div class="h5 mb-0 mt-2 font-weight-bold col-lg-6 text-right" style="color: #014D7F""><?= number_format($solde_boaAirtel, 0, ',', ' '); ?> Ar</div>
            </div>
          <div class="col-auto">
            <i class="fas fa-calendar fa-2x text-gray-300"></i>
          </div>
        </div>
      </div>
    </div>
  </div>
  <div class="col-lg-4 mb-4">
    <div class="card" style="border-radius:15px; border-left-color: #FF7900; box-shadow: 0 2px 2px rgba(0, 0, 0, 0.50); border-width: 2px;">
      <div class="card-header">
        <div class="row">
          <div class="col-lg-1">
            <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
          </div>
          <div class="col-lg-6">
            <h5 style="color: #312B81"><i class="bx" style="font-size: 22px; margin-left: -20px;"> Solde actuel </i></h5> 
          </div>
        </div>
      </div>
      <div class="card-body">
        <div class="row">
              <div class="row py-2">
                  <div class="col-lg-6">
                    <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 134 45" width="100" height="30">
                      <defs><style>.cls-1{fill:#ff7900;}</style></defs>
                      <g id="Titles">
                      <path d="M26.66,10.17H8a3.79,3.79,0,1,0,0,7.57h9.5L2.22,33a3.78,3.78,0,1,0,5.35,5.35l15.3-15.3V32.6a3.79,3.79,0,0,0,7.57,0V14A3.78,3.78,0,0,0,26.66,10.17Z"/>
                      <path class="cls-1" d="M38,36.38H56.66a3.79,3.79,0,0,0,0-7.57H47.15l15.3-15.3A3.78,3.78,0,1,0,57.1,8.16L41.8,23.46V14a3.79,3.79,0,0,0-7.57,0V32.6A3.78,3.78,0,0,0,38,36.38Z"/>
                      <path d="M77.06,4.5c4.17,0,6.62,3.11,6.62,7.11s-2.45,7-6.62,7-6.62-3.12-6.62-7S72.89,4.5,77.06,4.5Zm0,11.62c2.66,0,3.66-2.27,3.66-4.51S79.72,7,77.06,7,73.4,9.25,73.4,11.61,74.4,16.12,77.06,16.12Z"/>
                      <path d="M84.94,8.54h2.54v1.81h0a3.44,3.44,0,0,1,3.11-2.08,1.92,1.92,0,0,1,.59.1v2.49a4.64,4.64,0,0,0-1-.1c-2,0-2.65,1.42-2.65,3.13v4.4H84.94Z"/>
                      <path d="M91.69,11.53c.15-2.5,2.39-3.26,4.58-3.26,1.94,0,4.28.44,4.28,2.77v5.08a5,5,0,0,0,.34,2.17H98.17a3.89,3.89,0,0,1-.18-.95,4.48,4.48,0,0,1-3.29,1.21c-1.84,0-3.32-.92-3.32-2.92,0-2.21,1.66-2.74,3.32-3s3.17-.19,3.17-1.29-.79-1.32-1.73-1.32-1.68.42-1.78,1.47Zm6.18,2a5.21,5.21,0,0,1-2.22.57c-.83.17-1.59.45-1.59,1.43s.78,1.25,1.64,1.25a2,2,0,0,0,2.17-2.25Z"/>
                      <path d="M102.12,8.54h2.54V9.89h.06a3.49,3.49,0,0,1,3-1.62c2.85,0,3.57,1.6,3.57,4v6h-2.68V12.78c0-1.6-.47-2.4-1.72-2.4-1.45,0-2.07.82-2.07,2.8v5.11h-2.68Z"/>
                      <path d="M122.05,17.66c0,1.61-.57,4.32-5.06,4.32-1.92,0-4.17-.9-4.3-3.13h2.66a1.82,1.82,0,0,0,2,1.34c1.49,0,2.17-1,2.15-2.41V16.49h0A3.28,3.28,0,0,1,116.54,18c-2.92,0-4.17-2.22-4.17-4.9,0-2.53,1.45-4.83,4.19-4.83a3,3,0,0,1,2.9,1.57h0V8.54h2.55Zm-2.55-4.37c0-1.59-.55-3-2.28-3-1.51,0-2.17,1.32-2.17,2.77s.53,2.93,2.17,2.93S119.5,14.68,119.5,13.29Z"/>
                      <path d="M125.84,14.06c.07,1.7.9,2.47,2.39,2.47a2.22,2.22,0,0,0,2.12-1.26h2.35a4.38,4.38,0,0,1-4.56,3.28,4.76,4.76,0,0,1-5-5.13,4.89,4.89,0,0,1,5-5.15c3.32,0,4.92,2.79,4.73,5.79Zm4.35-1.7c-.24-1.35-.83-2.07-2.13-2.07a2.11,2.11,0,0,0-2.22,2.07Z"/>
                      <path d="M70.44,22.6h4.17l3.15,9.26h0l3-9.26h4.16V36.07H82.17V26.52h0l-3.3,9.55H76.55l-3.3-9.45h0v9.45H70.44Z"/>
                      <path d="M91.16,26.05a4.79,4.79,0,0,1,5,5.15,5,5,0,1,1-10.09,0A4.79,4.79,0,0,1,91.16,26.05Zm0,8.26c1.83,0,2.38-1.56,2.38-3.11s-.55-3.13-2.38-3.13S88.8,29.64,88.8,31.2,89.35,34.31,91.16,34.31Z"/>
                      <path d="M97.36,26.32h2.55v1.36H100a3.46,3.46,0,0,1,3-1.63c2.84,0,3.56,1.61,3.56,4v6h-2.68V30.56c0-1.6-.47-2.39-1.71-2.39-1.46,0-2.08.81-2.08,2.79v5.11H97.36Z"/>
                      <path d="M110.22,31.84c.07,1.7.9,2.47,2.39,2.47a2.22,2.22,0,0,0,2.12-1.26h2.35a4.38,4.38,0,0,1-4.56,3.28,4.76,4.76,0,0,1-5-5.13,4.89,4.89,0,0,1,5-5.15c3.32,0,4.92,2.79,4.73,5.79Zm4.35-1.69c-.24-1.36-.83-2.08-2.13-2.08a2.11,2.11,0,0,0-2.22,2.08Z"/>
                      <path d="M123,37.29a3.06,3.06,0,0,1-3.36,2.21c-.55,0-1.09,0-1.64-.09V37.2a13.51,13.51,0,0,0,1.57.09,1.26,1.26,0,0,0,.92-1.83L117,26.32h2.86L122.09,33h0l2.13-6.67H127Z"/>
                    </g>
                    </svg>
                  </div>
                  
                <div class="text-xs col-lg-6 font-weight-bold text-right " style="color: #00894A"> BOA </div>
                <div class="h5 mb-0 font-weight-bold text-gray-800 col-lg-6" style="color: #014D7F""><?= number_format($soldeOrange, 0, ',', ' '); ?> Ar</div>
                <div class="h5 mb-0 font-weight-bold text-gray-800 col-lg-6 text-right" style="color: #014D7F""><?= number_format($princ_solde, 0, ',', ' '); ?> Ar</div>
              </div>
            
                 
          
          <div class="col-auto">
            <i class="fas fa-calendar fa-2x text-gray-300"></i>
          </div>
        </div>
      </div>
    </div>
  </div>
        <div class="col-lg-4 mb-4">
            <div class="card" style="border-radius:15px; border-left-color: green;  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.50); border-width: 2px;">
            <div class="card-header">
            <div class="row">
              <div class="col-lg-1">
                <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
              </div>
              <div class="col-lg-6">
              
                <h5 style="color: #312B81">
                  <i class="bx" style="font-size: 22px; margin-left: -20px;">Solde actuel</i>
                </h5> 
              </div>
            </div>
            </div>
              <div class="card-body">
            <div class="row">
                <div class="row py-2">
                    <div class="col-lg-6">
                      <img src="<?= base_url() ?>assets/images/logo_MVOLA.gif" alt="logo_mvola" style="width: 75px; height: 30px;"/>
                    </div>
                  <!-- <div class="text-xs font-weight-bold  text-uppercase mb-1 mr-auto" style="color: #FFDE00">Telma</div> -->              
                    <div class="col-lg-6 text-xs font-weight-bold text-right" style="color: #00894A;">BOA</div>
                    <div class="col-lg-6 h5 mb-0 mt-2 font-weight-bold" style="color: #014D7F"><?= number_format($soldetelma, 0, ',', ' '); ?> Ar</div>
                    <div class="col-lg-6 h5 mb-0 mt-2 font-weight-bold text-right" style="color: #014D7F"><?= number_format($solde_boaTelma, 0, ',', ' '); ?> Ar</div>
                  </div>
                  </div>
                
                <!-- <div class="row">
                    <div class="row py-2">
                        <div class="col-lg-6">
                          <img src="<?= base_url() ?>assets/images/logo_MVOLA.gif" alt="logo_mvola" style="width: 75px; height: 30px;"/>
                        </div>
                        <div class="col-lg-6 text-xs text-right font-weight-bold" style="color: #00894A;">BOA</div>
                        <div class="col-lg-6 h5 mb-0 mt-2 text-right  font-weight-bold" style="color: #014D7F;"><?= number_format($soldetelma, 0, ',', ' '); ?></div>
                        <div class="col-lg-6 h5 mb-0 mt-2 text-right font-weight-bold" style="color: #014D7F;"><?= number_format($solde_boaTelma, 0, ',', ' '); ?></div>
                    </div>
                </div> -->
              </div>
            </div>
        </div>
    <div class="col-lg-4 mb-4">
      <div class="card" style="border-radius:15px; border-left-color: #B80000; box-shadow: 0 2px 2px rgba(0, 0, 0, 0.50); border-width: 2px;">
        <div class="card-header">
          <div class="row">
            <div class="col-lg-1">
              <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
            </div>
            <div class="col-lg-6">
              <h5 style="color: #312B81"><i class="bx" style="font-size: 22px; margin-left: -20px;"> Solde sur l'ecart </i></h5> 
            </div>
          </div>
        </div>
          <div class="card-body">
            <div class="row">
              <div class="row py-2">
                <div class="col-lg-6">
                  <img class="mb-1 mr-auto" src="<?= base_url()?>assets/images/airtel-logo.png" alt="airtel_logo" style="width: 75px; height: 20px;">
                </div>
                <div class="col-lg-6 text-xs font-weight-bold text-right" style="color: #00894A">BOA</div>
                <div class="col-lg-6 mt-3 h5 mb-0 font-weight-bold" style="color: #014D7F"><?= number_format($solde_ecart_airtel, 0, ',', ' ') ?> Ar</div>
                <!-- <div class="col-lg-6 mt-3 h5 mb-0 font-weight-bold text-right" style="color: #014D7F"> -1 500 225 000 Ar</div> -->
                  <div class="col-lg-6 mt-3 h5 mb-0 font-weight-bold text-right" style="color: #014D7F" style="color: #014D7F"><?= number_format($solde_ecart_boa_airtel, 0, ',', ' ') ?> Ar</div> 
                </div>
            </div>
          </div>
        </div>      
      </div>

    <div class="col-lg-4 mb-4">
      <div class="card" style="border-radius:15px; border-left-color: #FF7900; box-shadow: 0 2px 2px rgba(0, 0, 0, 0.50); border-width: 2px;">
        <div class="card-header">
          <div class="row">
            <div class="col-lg-1">
              <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
            </div>
            <div class="col-lg-6">
              <h5 style="color: #312B81"><i class="bx" style="font-size: 22px; margin-left: -20px;"> Solde sur l'ecart </i></h5> 
            </div>
          </div>
        </div>
          <div class="card-body">
            <div class="row">
                <div class="row py-2">
                  <div class="col-lg-6">
                    <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 134 45" width="100" height="30">
                        <defs><style>.cls-1{fill:#ff7900;}</style></defs>
                        <g id="Titles">
                        <path d="M26.66,10.17H8a3.79,3.79,0,1,0,0,7.57h9.5L2.22,33a3.78,3.78,0,1,0,5.35,5.35l15.3-15.3V32.6a3.79,3.79,0,0,0,7.57,0V14A3.78,3.78,0,0,0,26.66,10.17Z"/>
                        <path class="cls-1" d="M38,36.38H56.66a3.79,3.79,0,0,0,0-7.57H47.15l15.3-15.3A3.78,3.78,0,1,0,57.1,8.16L41.8,23.46V14a3.79,3.79,0,0,0-7.57,0V32.6A3.78,3.78,0,0,0,38,36.38Z"/>
                        <path d="M77.06,4.5c4.17,0,6.62,3.11,6.62,7.11s-2.45,7-6.62,7-6.62-3.12-6.62-7S72.89,4.5,77.06,4.5Zm0,11.62c2.66,0,3.66-2.27,3.66-4.51S79.72,7,77.06,7,73.4,9.25,73.4,11.61,74.4,16.12,77.06,16.12Z"/>
                        <path d="M84.94,8.54h2.54v1.81h0a3.44,3.44,0,0,1,3.11-2.08,1.92,1.92,0,0,1,.59.1v2.49a4.64,4.64,0,0,0-1-.1c-2,0-2.65,1.42-2.65,3.13v4.4H84.94Z"/>
                        <path d="M91.69,11.53c.15-2.5,2.39-3.26,4.58-3.26,1.94,0,4.28.44,4.28,2.77v5.08a5,5,0,0,0,.34,2.17H98.17a3.89,3.89,0,0,1-.18-.95,4.48,4.48,0,0,1-3.29,1.21c-1.84,0-3.32-.92-3.32-2.92,0-2.21,1.66-2.74,3.32-3s3.17-.19,3.17-1.29-.79-1.32-1.73-1.32-1.68.42-1.78,1.47Zm6.18,2a5.21,5.21,0,0,1-2.22.57c-.83.17-1.59.45-1.59,1.43s.78,1.25,1.64,1.25a2,2,0,0,0,2.17-2.25Z"/>
                        <path d="M102.12,8.54h2.54V9.89h.06a3.49,3.49,0,0,1,3-1.62c2.85,0,3.57,1.6,3.57,4v6h-2.68V12.78c0-1.6-.47-2.4-1.72-2.4-1.45,0-2.07.82-2.07,2.8v5.11h-2.68Z"/>
                        <path d="M122.05,17.66c0,1.61-.57,4.32-5.06,4.32-1.92,0-4.17-.9-4.3-3.13h2.66a1.82,1.82,0,0,0,2,1.34c1.49,0,2.17-1,2.15-2.41V16.49h0A3.28,3.28,0,0,1,116.54,18c-2.92,0-4.17-2.22-4.17-4.9,0-2.53,1.45-4.83,4.19-4.83a3,3,0,0,1,2.9,1.57h0V8.54h2.55Zm-2.55-4.37c0-1.59-.55-3-2.28-3-1.51,0-2.17,1.32-2.17,2.77s.53,2.93,2.17,2.93S119.5,14.68,119.5,13.29Z"/>
                        <path d="M125.84,14.06c.07,1.7.9,2.47,2.39,2.47a2.22,2.22,0,0,0,2.12-1.26h2.35a4.38,4.38,0,0,1-4.56,3.28,4.76,4.76,0,0,1-5-5.13,4.89,4.89,0,0,1,5-5.15c3.32,0,4.92,2.79,4.73,5.79Zm4.35-1.7c-.24-1.35-.83-2.07-2.13-2.07a2.11,2.11,0,0,0-2.22,2.07Z"/>
                        <path d="M70.44,22.6h4.17l3.15,9.26h0l3-9.26h4.16V36.07H82.17V26.52h0l-3.3,9.55H76.55l-3.3-9.45h0v9.45H70.44Z"/>
                        <path d="M91.16,26.05a4.79,4.79,0,0,1,5,5.15,5,5,0,1,1-10.09,0A4.79,4.79,0,0,1,91.16,26.05Zm0,8.26c1.83,0,2.38-1.56,2.38-3.11s-.55-3.13-2.38-3.13S88.8,29.64,88.8,31.2,89.35,34.31,91.16,34.31Z"/>
                        <path d="M97.36,26.32h2.55v1.36H100a3.46,3.46,0,0,1,3-1.63c2.84,0,3.56,1.61,3.56,4v6h-2.68V30.56c0-1.6-.47-2.39-1.71-2.39-1.46,0-2.08.81-2.08,2.79v5.11H97.36Z"/>
                        <path d="M110.22,31.84c.07,1.7.9,2.47,2.39,2.47a2.22,2.22,0,0,0,2.12-1.26h2.35a4.38,4.38,0,0,1-4.56,3.28,4.76,4.76,0,0,1-5-5.13,4.89,4.89,0,0,1,5-5.15c3.32,0,4.92,2.79,4.73,5.79Zm4.35-1.69c-.24-1.36-.83-2.08-2.13-2.08a2.11,2.11,0,0,0-2.22,2.08Z"/>
                        <path d="M123,37.29a3.06,3.06,0,0,1-3.36,2.21c-.55,0-1.09,0-1.64-.09V37.2a13.51,13.51,0,0,0,1.57.09,1.26,1.26,0,0,0,.92-1.83L117,26.32h2.86L122.09,33h0l2.13-6.67H127Z"/>
                      </g>
                    </svg>
                  </div>
                    <div class="text-xs font-weight-bold text-right col-lg-6" style="color: #00894A">BOA</div>
                    <div class="h5 mb-0 col-lg-6 mt-1 font-weight-bold " style="color: #014D7F; "><?= number_format($solde_ecart_orange, 0, ',', ' ') ?> Ar</div>
                    <div class="h5 mb-0 col-lg-6 mt-1 font-weight-bold text-right" style="color: #014D7F;"><?= number_format($solde_ecart_princ, 0, ',', ' ')?>Ar</div>
                  </div>
                  </div>
                </div>
              </div>
        </div>
        <div class="col-lg-4 mb-4">
      <div class="card" style="border-radius:15px; border-left-color: green; box-shadow: 0 2px 2px rgba(0, 0, 0, 0.50); border-width: 2px;">
        <div class="card-header">
          <div class="row">
            <div class="col-lg-1">
              <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
            </div>
            <div class="col-lg-6">
              <h5 style="color: #312B81"><i class="bx" style="font-size: 22px; margin-left: -20px;"> Solde sur l'ecart </i></h5> 
            </div>
          </div> 
        </div>
          <div class="card-body">
            <div class="row">
                <div class="row py-2">
                    <div class="col-lg-6">
                      <img src="<?= base_url() ?>assets/images/logo_MVOLA.gif" alt="logo_mvola" style="width: 75px; height: 30px;"/>
                    </div>
                  <!-- <div class="text-xs font-weight-bold  text-uppercase mb-1 mr-auto" style="color: #FFDE00">Telma</div> -->              
                    <div class="col-lg-6 text-xs font-weight-bold text-right" style="color: #00894A">BOA</div>
                    <div class="col-lg-6 h5 mb-0 mt-2 font-weight-bold" style="color: #014D7F"><?= number_format($solde_ecart_telma, 0, ',', ' '); ?> Ar</div>
                    <div class="col-lg-6 h5 mb-0 mt-2 font-weight-bold text-right" style="color: #014D7F"><?= number_format($solde_ecart_boa_telma, 0, ',', ' ');?> Ar</div>
                  </div>
                  </div>
                </div>
              </div>
        </div>
    </div>
    </div>
    </div>
</div>
</div>

<!-- Ecart  -->



<!-- Chart 1 -->
<div class="container-fluid">
  <div class="row">
  <div class="col-lg-6 mt-4">
    <div class="card" style="border-radius:10px; box-shadow: 0 0 7px rgba(0, 0, 0, 0.50);">
      <div class="card-header">
        <div class="row">
          <div class="col-lg-1">
            <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
          </div>
          <div class="col-lg-6">
            <h5 style="color: #312B81">
              <i class="bx" style="font-size: 22px; margin-left: -20px;">Solde mensuel</i>
            </h5> 
          </div>
        </div>
      </div>
      <div class="card-body">
        <div class="row">
          <canvas id="myChart4"></canvas>
        </div>
      </div>
    </div>
  </div>


  <div class="col-lg-6 mt-4">
    <div class="card" style="border-radius:10px; box-shadow: 0 0 7px rgba(0, 0, 0, 0.50);">
      <div class="card-header">
        <div class="row">
          <div class="col-lg-1">
            <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
          </div>
          <div class="col-lg-6">
            <h5 style="color: #312B81">
              <i class="bx" style="font-size: 22px; margin-left: -20px;">Solde mensuel</i>
            </h5> 
          </div>
        </div>
      </div>
      <div class="card-body">
        <div class="row">
          <canvas id="myChart5"></canvas>
        </div>
      </div>
    </div>
  </div>

  <div class="col-lg-6 mt-4">
    <div class="card" style="border-radius:10px; box-shadow: 0 0 7px rgba(0, 0, 0, 0.50);">
      <div class="card-header">
        <div class="row">
          <div class="col-lg-1">
            <i class='bx bxs-dollar-circle' style="color: #312B81; font-size: 25px; margin-left: -15px;"></i>
          </div>
          <div class="col-lg-6">
            <h5 style="color: #312B81">
              <i class="bx" style="font-size: 22px; margin-left: -20px;">Solde mensuel</i>
            </h5> 
          </div>
        </div>
      </div>
      <div class="card-body">
        <div class="row">
          <canvas id="myChart6"></canvas>
        </div>
      </div>
    </div>
  </div>
</div>

</div>




</div>



</div>
  
<script>


var chart_data= <?= json_encode($chart_data) ?>;
let airtel_datasets= Array.from({ length: 12 }, () => 0.0);
let telma_datasets= Array.from({ length: 12 }, () => 0.0);
let orange_datasets= Array.from({ length: 12 }, () => 0.0);
let princ_datasets= Array.from({ length: 12 }, () => 0.0);
let com_datasets= Array.from({ length: 12 }, () => 0.0);
let boa_telma_datasets= Array.from({ length: 12 }, () => 0.0);
let boa_airtel_datasets= Array.from({length : 12}, () => 0.0);
let solde_airtel_precedent;
let solde_telma_precedent;
let solde_boa_airtel_precedent;
let solde_boa_telma_precedent;
let solde_orange_precedent;
let solde_princ_precedent;
let solde_comm_precedent;

console.log(chart_data);

for(let i=1; i<=12; i++) {
  solde_airtel_precedent=0;
  solde_boa_airtel_precedent=0;
  solde_telma_precedent=0;
  solde_boa_telma_precedent=0;
  solde_orange_precedent =0; 
  solde_princ_precedent=0; 
  solde_com_precedent =0;

  if(Array.isArray(chart_data["telma_line"][i])) {

    for (element of chart_data["telma_line"][i]) {
      solde_telma_precedent += parseInt(element.solde);
      solde_boa_telma_precedent += parseInt(element.MONTANT);
    }

  } else {
    telma_datasets[i-1]= 0;
    boa_telma_datasets[i-1] = 0;
  }
  
  if (Array.isArray(chart_data["airtel_line"][i])) {

    for (element of chart_data["airtel_line"][i]) {
      solde_airtel_precedent += parseInt(element.solde);
      solde_boa_airtel_precedent += parseInt(element.MONTANT);
    }
  } else {
    airtel_datasets[i-1]=0;
    boa_airtel_datasets[i-1] =0;
  }

  if (Array.isArray(chart_data["orange_line"][i])) {
    for (element of chart_data["orange_line"][i]) {
      
      // console.log(`${element.principale} + ${solde_princ_precedent} =  ${solde_princ_precedent}`);
      solde_orange_precedent += parseInt(element.orange);
      solde_princ_precedent += parseInt(element.principale);
      solde_com_precedent += parseInt(element.commission);
    }
  } else {
    orange_datasets[i-1]=0;
    princ_datasets[i-1] =0;
    com_datasets[i-1] =0;
  }

  airtel_datasets[i-1]= Math.abs(solde_airtel_precedent);
  boa_airtel_datasets[i-1]= Math.abs(solde_boa_airtel_precedent);
  telma_datasets[i-1]= Math.abs(solde_telma_precedent);
  boa_telma_datasets[i-1]= Math.abs(solde_boa_telma_precedent);
  orange_datasets[i-1]= Math.abs(solde_orange_precedent);
  princ_datasets[i-1]= Math.abs(solde_princ_precedent);
  com_datasets[i-1]= Math.abs(solde_com_precedent);
}



const month= ["Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Decembre"];
const dataLine1 = {
  labels: month,
  datasets: [{
    label: 'Airtel',
    data: airtel_datasets,
    fill: true,
    borderColor: '#b80000',
    tension: 0.5
  }, {
    label: 'BOA',
    data: boa_airtel_datasets,
    fill: true,
    borderColor: '#00894A',
    tension: 0.5
  }]
};
const dataLine2 = {
  labels: month,
  datasets: [{
    label: 'Orange',
    data: orange_datasets,
    fill: true,
    borderColor: '#FF7900',
    tension: 0.5
  }, {
    label: 'BOA',
    data: princ_datasets,
    fill: true,
    borderColor: '#00894A',
    tension: 0.5
  }]
};
const dataLine3 = {
  labels: month,
  datasets: [{
    label: 'Telma',
    data: telma_datasets,
    fill: true,
    borderColor: '#FFDE00',
    tension: 0.5
  }, {
    label: 'BOA',
    data: boa_telma_datasets,
    fill: true,
    borderColor: '#00894A',
    tension: 0.5
  }]
};

const configLine1= {
  type: "line",
  data: dataLine1
}
const configLine2= {
  type: "line",
  data: dataLine2
}
const configLine3= {
  type: "line",
  data: dataLine3
}


new Chart(document.getElementById('myChart4').getContext('2d'), configLine1);
new Chart(document.getElementById('myChart5').getContext('2d'), configLine2);
new Chart(document.getElementById('myChart6').getContext('2d'), configLine3);
</script>


