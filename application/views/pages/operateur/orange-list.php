


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
                        <th colspan="12" style="text-align: center ;">BOA</th>
                        <th style="text-align: center ;"></th>
                        <th colspan="9" style="text-align: center ;">ORANGE</th>
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
                      <th>Numero</th>
                      <th></th>
                      <th>Numero</th>
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
                  
                  <?php foreach($normal_ci as $item) { ?>
                    <tr>
                      <td><?php echo $item->comm_code_agence ?></td>
                      <td><?php echo $item->princ_date_oper ?></td>
                      <td><?php echo $item->princ_date_val ?></td>
                      <td><?php echo $item->comm_date_val ?></td>
                      <td><?php echo $item->princ_montant ?></td>
                      <td><?php echo $item->comm_montant ?></td>
                      <td><?php echo "" ?></td>
                      <td><?php echo $item->princ_devise?></td>
                      <td><?php echo $item->princ_oper ?></td>
                      <td><?php echo $item->princ_expl ?></td>
                      <td><?php echo $item->princ_ref_igor ?></td>
                      <td><?php echo substr($item->cle, 0, 10) ?></td>
                      <td></td>
                      <td><?php echo substr($item->cle, 0, 10) ?></td>
                      <td><?php echo $item->orange_date ?></td>
                      <td><?php echo $item->orange_heure ?></td>
                      <td><?php echo $item->orange_ref ?></td>
                      <td><?php echo $item->orange_service ?></td>
                      <td><?php echo $item->orange_num_compte ?></td>
                      <td><?php echo $item->orange_debit ?></td>
                      <td><?php echo $item->orange_credit ?></td>
                    </tr>
                  <?php  } ;?>
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
                  <th colspan="8" style="text-align: center ;">ORANGE</th>
              </tr>
              <tr>
                <th>Date_Oper</th>
                <th>date_val</th>
                <th>montant</th>
                <th>Devise</th>
                <th>Oper</th>
                <th>Expl</th>
                <th>ReferenceIgor</th>
                <th>Numero</th>
                <th></th>
                <th>Numero</th>
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
                <?php foreach($normal_co as $item) { ?>
                  <tr>
                    <td><?php echo $item->princ_date_oper ?></td>
                    <td><?php echo $item->princ_date_val ?></td>
                    <td><?php echo $item->princ_montant ?></td>
                    <td><?php echo $item->princ_devise ?></td>
                    <td><?php echo $item->princ_oper ?></td>
                    <td><?php echo $item->princ_expl ?></td>
                    <td><?php echo $item->princ_ref_igor ?></td>
                    <td><?php echo substr($item->cle, 0, 10); ?></td>
                    <td></td>
                    <td><?php echo substr($item->cle, 0, 10); ?></td>
                    <td><?php echo $item->orange_date ?></td>
                    <td><?php echo $item->orange_heure ?></td>
                    <td><?php echo $item->orange_ref ?></td>
                    <td><?php echo $item->orange_service ?></td>
                    <td><?php echo $item->orange_num_compte ?></td>
                    <td><?php echo $item->orange_debit ?></td>
                    <td><?php echo $item->orange_credit ?></td>
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
              <th colspan="12" style="text-align: center ;">BOA</th>
              <!-- <th style="text-align: center ;"></th>
              <th colspan="9" style="text-align: center ;">ORANGE</th>-->
              <th colspan="3">Régularisation</th> 
          </tr>
          <tr>
            <!-- <th></th> -->
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
            <th>Etat</th>
            <th>Date_regul</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>  
          <?php foreach($dat as $item) { ?> 
            <tr>
                    <td><?php echo $item->comm_code_agence ?></td>
                    <td><?php echo $item->princ_date_oper ?></td>
                    <td><?php echo $item->princ_date_val ?></td>
                    <td><?php echo $item->comm_date_val ?></td>
                    <td><?php echo $item->princ_montant ?></td>
                    <td><?php echo $item->comm_montant ?></td>
                    <td><?php echo "" ?></td>
                    <td><?php echo $item->princ_devise?></td>
                    <td><?php echo $item->princ_oper ?></td>
                    <td><?php echo $item->princ_expl ?></td>
                    <td><?php echo $item->princ_ref_igor ?></td>
                    <td><?php echo $item->cle ?></td>
                    <td>Non</td>
                    <td></td>
                    <td>
                        <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->princ_ref_igor ?>', 'Non', '')">
                            <box-icon name='printer'  ></box-icon>Modifier
                        </a> 
                       
                      </td>
                    
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
          <th colspan="9"> BOA</th>
          <th colspan="3"> Regularisation</th>
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
            <th>Etat</th>
            <th>Date_regul</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($cat as $item) { ?> 
            <tr>
              <td><?php echo $item->princ_date_oper ?></td>
              <td><?php echo $item->princ_date_val ?></td>
              <td><?php echo $item->princ_montant ?></td>
              <td><?php echo $item->princ_devise ?></td>
              <td><?php echo $item->princ_oper ?></td>
              <td><?php echo $item->princ_expl ?></td>
              <td><?php echo $item->princ_ref_igor ?></td>
              <td><?php echo $item->cle ?></td>
              <td>Non</td>
              <td></td>
              <td>
                <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->princ_ref_igor ?>', 'Non', '')">
                            <box-icon name='printer'  ></box-icon>Modifier
                </a> 
                <a href="javascript:void(0);" class="text-primary" data-target="#myModalFr10"
                              data-compte="<?php echo $item->princ_compte ?>"
                              data-date-oper="<?php echo $item->princ_date_oper ?>"
                              data-montant="<?php echo $item->princ_montant ?>"
                              data-libelle="<?php echo $item->princ_libelle ?>"
                              data-oper="<?php echo $item->princ_oper ?>"
                              data-ref-igor="<?php echo $item->princ_ref_igor ?>" data-toggle="modal">
                <box-icon name="printer"></box-icon>Imprimer
                </a>
              </td>
            </tr>
        <?php } ?> 
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
        <?php foreach($ind as $item) { ?> 
            <tr>
              <td></td>
              <td><?php echo $item->cle ?></td>
              <td></td>
              <td><?php echo $item->orange_date ?></td>
              <td><?php echo $item->orange_heure ?></td>
              <td><?php echo $item->orange_ref ?></td>
              <td><?php echo $item->orange_service ?></td>
              <td><?php echo $item->orange_num_compte ?></td>
              <td><?php echo $item->orange_debit ?></td>
            </tr>
          <?php } ?> 
        </tbody>
      </table>
    </div>     
  </div>


  <div id="tabVI" class="container tab-pane"><br>
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
            </tr>
            

          <?php } ?> 
              </tbody>
          </table>
      </div>
  </div>


<form id="modifFormfr10">
  <div class="modal fade" id="myModalFr10">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="card p-3">
          <div class="text-center" style="font-size: 17px;">
            <b>Fr Dix</b>
            <button type="button" class="close" data-dismiss="modal">&times;</button>
          </div>
            <div class="row form-group mt-2">
                <div class="col-sm-12">
                  <label for="responsable">Résponsable de L'unité</label>
                  <select name="responsable" id="responsable" class="form-control">
                    <option value="hasina">Mme Hasina ANDRIAMAMPIANINA</option>
                    <option value="laza">Mr Lazanirainy RANDRIANAIVOSON</option>
                  </select>
                </div>
                <div class="col-sm-12 mt-1">
                  <label for="nom">Nom initial</label>
                  <input type="text" id="nom" name="nom" class="form-control" required>
                </div>
                <div class="col-sm-12">
                  <label for="compte">Compte client</label>
                  <input type="text" id="compte" name="compte" class="form-control" required>
                </div>
                <div class="col-sm-12">
                  <label for="comm">Compte commission</label>
                  <input type="text" id="comm" name="comm" class="form-control" required>
                </div>
              </div>
                <div class="text-center">
                  <button type="button" class="btn btn-warning" data-dismiss="modal">Annuler</button>
                  <input type="submit" class="btn btn-info" value="Imprimer" />
                </div>
              </div>
            </div>
          </div>
    </div>
  </div>
</form>

<script>
  var compte, dateOper, montant, libelle, oper, refIgor;
  $("#myModalFr10").on('show.bs.modal', function(event) {
    link = $(event.relatedTarget);
    compte = link.data('compte');
    dateOper = link.data('date-oper');
    montant = link.data('montant').toLocaleString('fr-FR');
    libelle = link.data('libelle');
    oper = link.data('oper');
    refIgor = link.data('ref-igor');
});

$("#modifFormfr10").submit(function(e) {
    e.preventDefault();
    var responsable = $("#responsable").val();
    const nom= $("#nom").val();
    const compte_client= $("#compte").val();
    const comm= $("#comm").val();
    let place;

    if(responsable === "hasina") {
      responsable = document.getElementById("responsable").options[ document.getElementById("responsable").selectedIndex].text;
      place= "Directeur Déléguée à la Monétique";
    } else {
      responsable = document.getElementById("responsable").options[ document.getElementById("responsable").selectedIndex].text;
      place= " Directeur Déléguée à la Monétique (p.i)";
    }

    // console.log(compte, dateOper, montant, libelle, oper, refIgor);

    var url = '<?php echo site_url('Imprimer/imprimerDocument'); ?>' +
      '?date=' + encodeURIComponent(dateOper) +
      '&ref=' + encodeURIComponent(refIgor) +
      '&oper=' + encodeURIComponent(oper)+
      '&montant=' + encodeURIComponent(montant)+
      '&libelle=' + encodeURIComponent(libelle)+
      '&nom=' + encodeURIComponent(nom) +
      '&responsable=' + encodeURIComponent(responsable) + 
      '&compte_client=' + encodeURIComponent(compte_client) + 
      '&compte_comm=' + encodeURIComponent(comm)+
      '&compte=' + encodeURIComponent(compte)+
      "&operateur="  + encodeURIComponent("OM") +
      "&mobile_money=" + encodeURIComponent("M VOLA") +
      "&tax="  + encodeURIComponent("") +
      "&tva=" + encodeURIComponent("") +
      "&montant_comm=" + encodeURIComponent("") +
      "&place=" + encodeURIComponent(place)

    var nouvelleFenetre = window.open(url, '_blank'); 

});

</script>

<script>

$(document).ready(function(){

  $("#tableNormaleCashOut").DataTable({
    responsive: true,
    scrollX: true, // Active la barre de défilement horizontal
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
        scrollX: true,
        buttons: [
            'copy', 'csv', 'excel', 'pdf', 'print'
            ],
        language: {
          url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
    });

  $("#tableDat").DataTable({
    responsive: true,
    scrollX: true,

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
    scrollX: true,

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
