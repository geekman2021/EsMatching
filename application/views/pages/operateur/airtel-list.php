


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
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabAnormaleCashIn">Anomalie Cash in</a>
        </li>
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabAnormaleCashOut">Anormalie Cash Out</a>
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
                        <th colspan="10" style="text-align: center ;">Orange Cash In</th>
                        <th colspan="9" style="text-align: center ;">IGore Cash In</th>
                    </tr>
                    <tr>
                        <th>TransferId</th>
                        <th>TransferDate</th>
                        <th>AccountNo</th>
                        <th>SenderMsiSDN</th>
                        <th>DestMsiSDN</th>
                        <th>Amount</th>
                        <th>Description</th>
                        <th>ServiceName</th>
                        <th>ReferenceNumber</th>
                        <th>ExternalId</th>
                        <th>ReferenceIgor</th>
                        <th>Compte</th>
                        <th>DateOper</th>
                        <th>DateVal</th>
                        <th>Devise</th>
                        <th>Montant</th>
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
    <div id="tabNormaleCashOut" class="container tab-pane "><br>
        <div class="table-responsive">
            <table class="table table-bordered" id="tableNormaleCashOut">
                <thead>
                    <tr>
                        <th colspan="10" style="text-align: center ;">Orange Cash Out</th>
                        <th colspan="9" style="text-align: center ;">IGore Cash Out</th>
                    </tr>
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
                        <th>Compte</th>
                        <th>DateOper</th>
                        <th>DateVal</th>
                        <th>Devise</th>
                        <th>Montant</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                    </tr>
                </thead>
                <tbody>
                 
                </tbody>
            </table>
        </div>
         
    </div>
    <div id="tabAnormaleCashIn" class="container tab-pane "><br>
        
    <div class="table-responsive">
            <table class="table table-bordered" id="tableAnormaleCashIn">
                <thead style="text-align: center ;">
                    <tr>
                        <th colspan="10" style="text-align: center ;">Orange Cash In</th>
                        <th colspan="9" style="text-align: center ;">IGore Cash In</th>
                    </tr>
                    <tr>
                    <th>Transfer Id</th>
                        <th>Transfer Date</th>
                        <th>Account No</th>
                        <th>Sender MsiSDN</th>
                        <th>Dest MsiSDN</th>
                        <th>Amount</th>
                        <th>Description</th>
                        <th>Service Name</th>
                        <th>Reference Number</th>
                        <th>External Id</th>
                        <th>Reference Igor</th>
                        <th>Compte</th>
                        <th>Date Oper</th>
                        <th>Date Val</th>
                        <th>Devise</th>
                        <th>Montant</th>
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
    <div id="tabAnormaleCashOut" class="container tab-pane "><br>
        
    <div class="table-responsive">
        <table class="table table-bordered" id="tableAnormaleCashOut">
            <thead>
                <tr>
                    <th colspan="10" style="text-align: center ;">Orange Cash Out</th>
                    <th colspan="10" style="text-align: center ;">IGore Cash Out</th>
                </tr>
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
                    <th>Compte</th>
                    <th>DateOper</th>
                    <th>DateVal</th>
                    <th>Devise</th>
                    <th>Montant</th>
                    <th>Libelle</th>
                    <th>Operation</th>
                    <th>Expl</th>
                    <th>ReferenceIgor</th>
                </tr>
            </thead>
            <tbody>
                
            </tbody>
        </table>
    </div>
         
      </ul>
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

  $("#tableAnormaleCashOut").DataTable({
    responsive: true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

  $("#tableAnormaleCashIn").DataTable({
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
      $("#tabAnormaleCashIn").removeClass("show active");
      $("#tabAnormaleCashOut").removeClass("show active");
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
