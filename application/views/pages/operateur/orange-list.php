


<div class=" container mt-5">
  <!-- Nav tabs -->
  
    <ul class="nav nav-tabs">
            <li class="nav-item col-sm-6">
                <a class="nav-link" href="#normale">Normale</a>
            </li>
            <li class="nav-item col-sm-6">
                <a class="nav-link" href="#anormale">Anomalie</a>
            </li>
    </ul>
</div>

  <!-- Tab panes -->
  <div class="tab-content">
    <div id="normale" class="container tab-pane "><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashIn">Normale Cash in</a>
        </li>
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashOut">Normale Cash Out</a>
        </li>
      </ul>
    </div>
    <div id="anormale" class="container tab-pane fade"><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabDat">Débit à Tort</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabCat">Crédit à Tort</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabInd">IND01</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabVi">VI</a>
        </li>
      </ul>
    </div>
  </div>
  <div class="container tab-content">
    <div id="tabNormaleCashIn" class="tab-pane"><br>
        <div class="table-responsive">
            <table class="table table-bordered" id="tableNormaleCashIn">
                <thead style="text-align: center ;">
                    <tr>
                        <th colspan="12" style="text-align: center ;">BOA</th>
                        <th style="text-align: center ;"></th>
                        <th colspan="9" style="text-align: center ;">ORANGE/th>
                    </tr>
                    <tr>
                      <th>Code_Agence</th>
                      <th>Date_Oper</th>
                      <th>princ_date_val</th>
                      <th>comm_date_val</th>
                      <th>princ_montant</th>
                      <th>comm_montant</th>
                      <th>Montant</th>
                      <th>Devise</th>
                      <th>Oper</th>
                      <th>Expl</th>
                      <th>ReferenceIgor</th>
                      <th>Cle</th>
                      <th></th>
                      <th>Cle</th>
                      <th></th>
                      <th>Date</th>
                      <th>Heure</th>
                      <th>Réference</th>
                      <th>Service</th>
                      <th>Num_Compte</th>
                      <th>Débit</th>
                      <th>Crédit</th>
                    </tr>
                </thead>
                <tbody>
                </tbody>
            </table>
        </div>
    </div>
    <div id="tabNormaleCashOut" class="container tab-pane "><br>
        <div class="table-responsive">
            <table class="table table-bordered" id="tableNormaleCashOut">
            <thead style="text-align: center ;">
              <tr>
                  <th colspan="8" style="text-align: center ;">BOA</th>
                  <th style="text-align: center ;"></th>
                  <th colspan="8" style="text-align: center ;">ORANGE/th>
              </tr>
              <tr>
                <th>Date_Oper</th>
                <th>date_val</th>
                <th>montant</th>
                <th>Devise</th>
                <th>Oper</th>
                <th>Expl</th>
                <th>ReferenceIgor</th>
                <th>Cle</th>
                <th></th>
                <th>Cle</th>
                <th>Date</th>
                <th>Heure</th>
                <th>Réference</th>
                <th>Service</th>
                <th>Num_Compte</th>
                <th>Débit</th>
                <th>Crédit</th>
              </tr>
            </thead>
              <tbody>
              </tbody>
            </table>
        </div>
         
  </div>

  <div class="container tab-content">
  <div id="tabDat" class="container tab-pane "><br>   
    <div class="table-responsive">
      <table class="table table-bordered" id="tableDat">
        <thead style="text-align: center ;">
          <tr>
              <th colspan="12" style="text-align: center ;">BOA</th>
              <th style="text-align: center ;"></th>
              <th colspan="9" style="text-align: center ;">ORANGE/th>
          </tr>
          <tr>
            <th>Code_Agence</th>
            <th>Date_Oper</th>
            <th>princ_date_val</th>
            <th>comm_date_val</th>
            <th>princ_montant</th>
            <th>comm_montant</th>
            <th>Montant</th>
            <th>Devise</th>
            <th>Oper</th>
            <th>Expl</th>
            <th>ReferenceIgor</th>
            <th>Cle</th>
            <th></th>
            <th>Cle</th>
            <th></th>
            <th>Date</th>
            <th>Heure</th>
            <th>Réference</th>
            <th>Service</th>
            <th>Num_Compte</th>
            <th>Débit</th>
            <th>Crédit</th>
          </tr>
        </thead>
        <tbody>    
        </tbody>
      </table>
    </div>
  </div>
  <div id="tabCat" class="container tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableCat">
        <thead style="text-align: center ;">
          <tr>
              <th colspan="8" style="text-align: center ;">BOA</th>
              <th style="text-align: center ;"></th>
              <th colspan="8" style="text-align: center ;">ORANGE/th>
          </tr>
          <tr>
            <th>Date_Oper</th>
            <th>date_val</th>
            <th>montant</th>
            <th>Devise</th>
            <th>Oper</th>
            <th>Expl</th>
            <th>ReferenceIgor</th>
            <th>Cle</th>
            <th></th>
            <th>Cle</th>
            <th>Date</th>
            <th>Heure</th>
            <th>Réference</th>
            <th>Service</th>
            <th>Num_Compte</th>
            <th>Débit</th>
            <th>Crédit</th>
          </tr>
        </thead>
        <tbody>
        </tbody>
      </table>
    </div>     
  </div>

  <div id="tabInd" class="container tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableInd">
        <thead>
          <tr>
            <th></th>
            <th>Cle</th>
            <th></th>
            <th>Date</th>
            <th>Heure</th>
            <th>Réference</th>
            <th>Service</th>
            <th>Num_Compte</th>
            <th>Débit</th>
          </tr>
        </thead>
        <tbody>
        </tbody>
      </table>
    </div>     
  </div>
  <div id="tabVI" class="tab-pane"><br>
      <div class="table-responsive">
          <table class="table table-bordered" id="tableVI">
              <thead style="text-align: center ;">
                <tr>
                  <th>Compte</th>
                  <th>DateOper</th>
                  <th>DateVal</th>
                  <th>Montant</th>
                  <th>Devise</th>
                  <th>ReferenceIgor</th>
                  <th>Libelle</th>
                  <th>Operation</th>
                  <th>Expl</th>
                </tr>
              </thead>
              <tbody>
              </tbody>
          </table>
      </div>
  </div>
</div>
</div>
<script>
$(document).ready(function(){

  $("#tableNormaleCashOut").DataTable({
    responsive: true,
    "scrollX": true, // Active la barre de défilement horizontal
    "autoWidth": false,
    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

    $('#tableNormaleCashIn').DataTable( {
        dom: 'Bfrtip',
        buttons: [
            'copy', 'csv', 'excel', 'pdf', 'print'
            ],
        language: {
          url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
    });

  $("#tableDat").DataTable({
    responsive: true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

  $("#tableCat").DataTable({
    responsive: true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });


  $("#tableInd").DataTable({
    responsive: true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });

  $("#tableVI").DataTable({
    responsive: true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });
  


    // $("#tableAnormaleCashOut").DataTable();
  $(".nav-tabs a").click(function(){
    $(this).tab('show');
    // Ajoutez le code pour masquer la tab "Table Normale Cash In" ici
    if($(this).attr('href') === '#anormale') {
      $("#tabNormaleCashIn").removeClass("show active");
      $("#tabNormaleCashOut").removeClass("show active");
    }
    if($(this).attr('href') === '#normale') {
      $("#tabDat").removeClass("show active");
      $("#tabCat").removeClass("show active");
    }
  });
});
</script>










<!-- <script>

  $(document).ready(function () {
    $('#myTab a').click(function (e) {
      e.preventDefault();
      $(this).tab('show');
    });
  }); -->



</script>
