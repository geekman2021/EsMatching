


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
          <a class="nav-link" href="#tabAdmin">Admin</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabVI">VI</a>
        </li>
      </ul>
    </div>

  </div>
  <div class="container tab-content">
    
    <div id="tabNormaleCashIn" class="tab-pane"><br>
    <div class="row">
        <div class="col-lg-6">
            <label for="daDeb">Date Debut</label>
            <input class="form-control" type="date" name="dateDebNormCI" id="dateDebNormCI" />
        </div>
        <div class="col-sm-6">
            <label for="daDeb">Date Fin</label>
            <input class="form-control" type="date" name="dateFinNormCI" id="dateFinNormCI" />
        </div>
    </div>
        <div class="table-responsive mt-1">
            <table class="table table-bordered" id="tableNormaleCashIn">
                <thead style="text-align: center ;">
                    <tr>
                        <th colspan="10" style="text-align: center ;">BOA</th>
                        <th></th>
                        <th colspan="10" style="text-align: center ;">Telma</th>
                    </tr>
                    <tr>
                        
                        <th>Date_Oper</th>
                        <th>Date_Val</th>
                        <th>Devise</th>
                        <th>Montant</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>Cle</th>
                        <th></th>
                        <th>Date</th>
                        <th>Heure</th>
                        <th>Transfer_id</th>
                        <th>Type</th>
                        <th>Channel</th>
                        <th>Etat</th>
                        <th>Montant</th>
                        <th>sender</th>
                        <th>receiver</th>
                    </tr>
                </thead>
                <tbody>
                  <?php foreach($normal_ci as $item) { ?>
                    <td><?php echo $item->DATE_OPER ?></td>
                    <td><?php echo $item->DATE_VAL ?></td>
                    <td><?php echo $item->DEVISE ?></td>
                    <td><?php echo $item->MONTANT ?></td>
                    <td><?php echo $item->LIBELLE ?></td>
                    <td><?php echo $item->OPER ?></td>
                    <td><?php echo $item->EXPL ?></td>
                    <td><?php echo $item->REF_IGOR ?></td>
                    <td><?php echo $item->cle ?></td>
                    <td></td>
                    <td><?php echo $item->date_d ?></td>
                    <td><?php echo $item->cle ?></td>
                    <td><?php echo $item->cle ?></td>
                    <td><?php echo $item->cle ?></td>
                    <td><?php echo $item->cle ?></td>
                    <td><?php echo $item->cle ?></td>
                  <?php }?>
                </tbody>
            </table>
        </div>
    </div>



    <div id="tabNormaleCashOut" class="container tab-pane "><br>
        <div class="row">
            <div class="col-lg-6">
                <label for="daDeb">Date Debut</label>
                <input class="form-control" type="date" name="dateDebNormCI" id="dateDebNormCI" />
            </div>
            <div class="col-sm-6">
                <label for="daDeb">Date Fin</label>
                <input class="form-control" type="date" name="dateFinNormCI" id="dateFinNormCI" />
            </div>
        </div>
        <div class="table-responsive">
            <input type="date" name="dateDebNormCI" id="dateDebNormCI" />
            <input type="date" name="dateFinNormCI" id="dateFinNormCI" />
            <table class="table table-bordered" id="tableNormaleCashOut">
                <thead style="text-align: center ;">
                    <tr>
                        <th colspan="10" style="text-align: center ;">BOA</th>
                        <th></th>
                        <th colspan="10" style="text-align: center ;">Telma</th>
                    </tr>
                    <tr>
                        
                        <th>Compte</th>
                        <th>Date_Oper</th>
                        <th>Date_Val</th>
                        <th>Devise</th>
                        <th>Montant</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>Cle</th>
                        <th></th>
                        <th>Cle</th>
                        <th>Date</th>
                        <th>Heure</th>
                        <th>Transfer_id</th>
                        <th>Type</th>
                        <th>Channel</th>
                        <th>Etat</th>
                        <th>Montant</th>
                        <th>sender</th>
                        <th>receiver</th>
                    </tr>
                </thead>
                <tbody>
                 
                </tbody>
            </table>
        </div>
    </div>
</div>

  <div class="container tab-content">
  <div id="tabDat" class="container tab-pane "><br>  
    <div class="table-responsive">
      <table class="table table-bordered" id="tableDat">
        <thead style="text-align: center ;">
            <tr>
                <th colspan="10" style="text-align: center ;">BOA</th>
                <th></th>
                <th colspan="10" style="text-align: center ;">Telma</th>
            </tr>
            <tr>
                
                <th>Compte</th>
                <th>Date_Oper</th>
                <th>Date_Val</th>
                <th>Devise</th>
                <th>Montant</th>
                <th>Libelle</th>
                <th>Operation</th>
                <th>Expl</th>
                <th>ReferenceIgor</th>
                <th>Cle</th>
                <th></th>
                <th>Cle</th>
                <th>Date</th>
                <th>Heure</th>
                <th>Transfer_id</th>
                <th>Type</th>
                <th>Channel</th>
                <th>Etat</th>
                <th>Montant</th>
                <th>sender</th>
                <th>receiver</th>
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
            <th colspan="10" style="text-align: center ;">BOA</th>
            <th></th>
            <th colspan="10" style="text-align: center ;">Telma</th>
        </tr>
        <tr>
            <th>Compte</th>
            <th>Date_Oper</th>
            <th>Date_Val</th>
            <th>Devise</th>
            <th>Montant</th>
            <th>Libelle</th>
            <th>Operation</th>
            <th>Expl</th>
            <th>ReferenceIgor</th>
            <th>Cle</th>
            <th></th>
            <th>Cle</th>
            <th>Date</th>
            <th>Heure</th>
            <th>Transfer_id</th>
            <th>Type</th>
            <th>Channel</th>
            <th>Etat</th>
            <th>Montant</th>
            <th>sender</th>
            <th>receiver</th>
        </tr>
        </thead>
        <tbody>
        </tbody>
      </table>
    </div>     
  </div>

  <div id="tabAdmin" class="container tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableAdmin">
        <thead>
          <tr>
            <th></th>
            <th>Cle</th>
            <th>Date</th>
            <th>Heure</th>
            <th>Transfer_id</th>
            <th>Type</th>
            <th>Channel</th>
            <th>Etat</th>
            <th>Montant</th>
            <th>sender</th>
            <th>receiver</th>
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

  $("#tableAdmin").DataTable({
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
