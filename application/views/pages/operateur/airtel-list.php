


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
          <a class="nav-link" href="#tabAmbi">Ambiguous</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabDeallo">Deallocation</a>
        </li>
        <li class="nav-item col-sm-1">
          <a class="nav-link" href="#tabVI">VI</a>
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
                        <th colspan="9" style="text-align: center ;">BOA</th>
                        <th colspan="10" style="text-align: center ;">Airtel</th>
                    </tr>
                    <tr>
                        <th>Compte</th>
                        <th>DateOper</th>
                        <th>DateVal</th>
                        <th>Devise</th>
                        <th>Montant</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>ExternalId</th>
                        <th>TransferId</th>
                        <th>TransferDate</th>
                        <th>AccountNo</th>
                        <th>SenderMsiSDN</th>
                        <th>DestMsiSDN</th>
                        <th>Amount</th>
                        <th>Description</th>
                        <th>ServiceName</th>
                        <th>ReferenceNumber</th>
                    </tr>
                </thead>
                <tbody>
                  <?php foreach($normale_ci as $item) { ?>
                    <tr>
                      <td><?php echo $item->COMPTE ?></td>
                      <td><?php echo $item->DATE_OPER ?></td>
                      <td><?php echo $item->DATE_VAL ?></td>
                      <td><?php echo $item->DEVISE ?></td>
                      <td><?php echo $item->MONTANT ?></td>
                      <td><?php echo $item->LIBELLE ?></td>
                      <td><?php echo $item->OPER ?></td>
                      <td><?php echo $item->EXPL ?></td>
                      <td><?php echo $item->REF_IGOR ?></td>
                      <td><?php echo $item->external_id ?></td>
                      <td><?php echo $item->TRANSFER_ID ?></td>
                      <td><?php echo $item->transfer_date ?></td>
                      <td><?php echo $item->account_no ?></td>
                      <td><?php echo $item->sender_msisdn ?></td>
                      <td><?php echo $item->dest_msisdn ?></td>
                      <td><?php echo $item->amount ?></td>
                      <td><?php echo $item->description ?></td>
                      <td><?php echo $item->service_name ?></td>
                      <td><?php echo $item->reference_number ?></td>
                    </tr>

                  <?php } ?>
                </tbody>
            </table>
        </div>
    </div>
    <div id="tabNormaleCashOut" class="container tab-pane "><br>
        <div class="table-responsive">
            <table class="table table-bordered" id="tableNormaleCashOut">
                <thead>
                    <tr>
                        <th colspan="9" style="text-align: center ;">BOA</th>
                        <th colspan="10" style="text-align: center ;">AIRTEL</th>
                    </tr>
                    <tr>
                      <th>Compte</th>
                      <th>DateOper</th>
                      <th>DateVal</th>
                      <th>Devise</th>
                      <th>Montant</th>
                      <th>Libelle</th>
                      <th>Operation</th>
                      <th>Expl</th>
                      <th>ReferenceIgor</th>
                      <th>ExternalId</th>
                      <th>TransferId</th>
                      <th>TransferDate</th>
                      <th>AccountNo</th>
                      <th>SenderMsiSDN</th>
                      <th>DestMsiSDN</th>
                      <th>Amount</th>
                      <th>Description</th>
                      <th>ServiceName</th>
                      <th>ReferenceNumber</th>
                    </tr>
                </thead>
                <tbody>
                  <?php foreach($normale_co as $item) { ?>
                    <tr>
                      <td><?php echo $item->COMPTE ?></td>
                      <td><?php echo $item->DATE_OPER ?></td>
                      <td><?php echo $item->DATE_VAL ?></td>
                      <td><?php echo $item->DEVISE ?></td>
                      <td><?php echo $item->MONTANT ?></td>
                      <td><?php echo $item->LIBELLE ?></td>
                      <td><?php echo $item->OPER ?></td>
                      <td><?php echo $item->EXPL ?></td>
                      <td><?php echo $item->REF_IGOR ?></td>
                      <td><?php echo $item->external_id ?></td>
                      <td><?php echo $item->TRANSFER_ID ?></td>
                      <td><?php echo $item->transfer_date ?></td>
                      <td><?php echo $item->account_no ?></td>
                      <td><?php echo $item->sender_msisdn ?></td>
                      <td><?php echo $item->dest_msisdn ?></td>
                      <td><?php echo $item->amount ?></td>
                      <td><?php echo $item->description ?></td>
                      <td><?php echo $item->service_name ?></td>
                      <td><?php echo $item->reference_number ?></td>
                    </tr>

                  <?php } ?>
                 
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
                <th colspan="10" style="text-align: center ;">BOA</th>
                <th colspan="9" style="text-align: center ;">Airtel</th>
                <th colspan="3" style="text-align: center ;">Regularisation</th>
            </tr>
            <tr>
              <th>Compte</th>
              <th>Date_Oper</th>
              <th>Date_Val </th>
              <th>Devise</th>
              <th>Montant</th>
              <th>Libelle</th>
              <th>Operation</th>
              <th>Expl</th>
              <th>ReferenceIgor</th>
              <th>ExternalId</th>
              <th>TransferId</th>
              <th>TransferDate</th>
              <th>AccountNo</th>
              <th>SenderMsiSDN</th>
              <th>DestMsiSDN</th>
              <th>Amount</th>
              <th>Description</th>
              <th>ServiceName</th>
              <th>ReferenceNumber</th>
              <th>Etat</th>
              <th>Date</th>
              <th>Action</th>
            </tr>
        </thead>
        <tbody>
          <?php foreach($igor_anomalie_ci as $item) { ?>
              <tr>
                <td><?php echo $item->COMPTE ?></td>
                <td><?php echo $item->DATE_OPER ?></td>
                <td><?php echo $item->DATE_VAL ?></td>
                <td><?php echo $item->DEVISE ?></td>
                <td><?php echo $item->MONTANT ?></td>
                <td><?php echo $item->LIBELLE ?></td>
                <td><?php echo $item->OPER ?></td>
                <td><?php echo $item->EXPL ?></td>
                <td><?php echo $item->REF_IGOR ?></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td>
                <a href="javascript:void(0);" class="text-warning">
                  <box-icon name="edit"></box-icon>
                </a>
                <a href="javascript:void(0);" class="text-primary" onClick="ouvrirEtImprimer('<?php echo $item->DATE_OPER ?>? ', '<?php echo $item->REF_IGOR ?>?', '<?php echo $item->MONTANT ?>', '<?php echo $item->OPER ?>', '<?php echo $item->COMPTE ?>')">
                ssdsds<box-icon name="printer"></box-icon>
                </a>
              </td>
              </tr>
          <?php } ?>
          <?php foreach($anomalie_ci as $item) { ?>
            <tr>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td><?php echo $item->external_id ?></td>
              <td><?php echo $item->TRANSFER_ID ?></td>
              <td><?php echo $item->transfer_date ?></td>
              <td><?php echo $item->account_no ?></td>
              <td><?php echo $item->sender_msisdn ?></td>
              <td><?php echo $item->dest_msisdn ?></td>
              <td><?php echo $item->amount ?></td>
              <td><?php echo $item->description ?></td>
              <td><?php echo $item->service_name ?></td>
              <td><?php echo $item->reference_number ?></td>
              <td></td>
              <td></td>
              
            </tr>
          <?php } ?>
        </tbody>
      </table>
    </div>
  </div>
  <div id="tabCat" class="container tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableCat">
      <thead style="text-align: center ;">
            <tr>
                <th colspan="10" style="text-align: center ;">BOA</th>
                <th colspan="9" style="text-align: center ;">Airtel</th>
                <th colspan="3" style="text-align: center ;">Regularisation</th>
            </tr>
            <tr>
              <th>Compte</th>
              <th>DateOper</th>
              <th>DateVal</th>
              <th>Devise</th>
              <th>Montant</th>
              <th>Libelle</th>
              <th>Operation</th>
              <th>Expl</th>
              <th>ReferenceIgor</th>
              <th>ExternalId</th>
              <th>TransferId</th>
              <th>TransferDate</th>
              <th>AccountNo</th>
              <th>SenderMsiSDN</th>
              <th>DestMsiSDN</th>
              <th>Amount</th>
              <th>Description</th>
              <th>ServiceName</th>
              <th>ReferenceNumber</th>
              <th>Etat</th>
              <th>Date</th>
              <th>Action</th>
            </tr>
        </thead>
        <tbody>
          <?php foreach($igor_anomalie_co as $item) { ?>
              <tr>
                <td><?php echo $item->COMPTE ?></td>
                <td><?php echo $item->DATE_OPER ?></td>
                <td><?php echo $item->DATE_VAL ?></td>
                <td><?php echo $item->DEVISE ?></td>
                <td><?php echo $item->MONTANT ?></td>
                <td><?php echo $item->LIBELLE ?></td>
                <td><?php echo $item->OPER ?></td>
                <td><?php echo $item->EXPL ?></td>
                <td><?php echo $item->REF_IGOR ?></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
              </tr>
          <?php } ?>
          <?php foreach($anomalie_co as $item) { ?>
            <tr>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td><?php echo $item->external_id ?></td>
              <td><?php echo $item->TRANSFER_ID ?></td>
              <td><?php echo $item->transfer_date ?></td>
              <td><?php echo $item->account_no ?></td>
              <td><?php echo $item->sender_msisdn ?></td>
              <td><?php echo $item->dest_msisdn ?></td>
              <td><?php echo $item->amount ?></td>
              <td><?php echo $item->description ?></td>
              <td><?php echo $item->service_name ?></td>
              <td><?php echo $item->reference_number ?></td>
              <td></td>
              <td></td>
              <td></td>
            </tr>
          <?php } ?>
        </tbody>
      </table>
    </div>     
  </div>

  <div id="tabAmbi" class="container tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableAmbi">
        <thead>
          <tr>
              <th>TransferId</th>
              <th>TransferDate</th>
              <th>ExternalId</th>
              <th>AccountNo</th>
              <th>SenderMsiSDN</th>
              <th>DestMsiSDN</th>
              <th>Amount</th>
              <th>Description</th>
              <th>ServiceName</th>
              <th>ReferenceNumber</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach ($ambiguous as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo $item->amount ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
          <?php } ?>

        </tbody>
      </table>
    </div>     
  </div>

  <div id="tabDeallo" class="container tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableDeallo">
      <thead>
          <tr>
              <th>TransferId</th>
              <th>TransferDate</th>
              <th>ExternalId</th>
              <th>AccountNo</th>
              <th>SenderMsiSDN</th>
              <th>DestMsiSDN</th>
              <th>Amount</th>
              <th>Description</th>
              <th>ServiceName</th>
              <th>ReferenceNumber</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach ($deallocation as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo $item->amount ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
          <?php } ?>

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
                <?php foreach($vi as $item) { ?>
                  <td><?php echo $item->COMPTE ?></td>
                  <td><?php echo $item->DATE_OPER ?></td>
                  <td><?php echo $item->DATE_VAL ?></td>
                  <td><?php echo $item->MONTANT ?></td>
                  <td><?php echo $item->DEVISE ?></td>
                  <td><?php echo $item->REF_IGOR ?></td>
                  <td><?php echo $item->LIBELLE ?></td>
                  <td><?php echo $item->OPER ?></td>
                  <td><?php echo $item->EXPL ?></td>
                <?php } ?>
              </tbody>
          </table>
      </div>
  </div>
</div>
</div>
<script>
   function ouvrirEtImprimer(date_oper, ref_igor, montant, oper, compte) {

    var url = '<?php echo site_url('Imprimer/imprimerDocument'); ?>' +
      '?date=' + encodeURIComponent(date_oper) +
      '&ref=' + encodeURIComponent(ref_igor) +
      '&oper=' + encodeURIComponent(oper)+
      '&montant=' + encodeURIComponent(montant)+
      '&compte=' + encodeURIComponent(compte);

    var nouvelleFenetre = window.open(url, '_blank');

  }


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

  $("#tableDeallo").DataTable({
    responsive: true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });

  $("#tableAmbi").DataTable({
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
