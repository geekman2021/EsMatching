


<div class=" container-fluid mt-5">
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
    <div id="normale" class="container-fluid tab-pane "><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashIn">Normale Cash in</a>
        </li>
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashOut">Normale Cash Out</a>
        </li>
      </ul>
    </div>
    <div id="anormale" class="container-fluid tab-pane fade"><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabDat">Débit à Tort</a>
        </li>
        <li class="nav-item col-sm-3">
          <a class="nav-link" href="#tabCat">Crédit à Tort</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabMvts">Mvts</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabInd">IND01</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabVI">VI</a>
        </li>
      </ul>
    </div>
  </div>

  <div class="container-fluid tab-content">
    <div id="tabNormaleCashIn" class="tab-pane"><br>
        <div class="table-responsive">
          <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste Normale CashIn</b>
              </div>
            </div>
          </div>
          <div class="card-body">
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
            
      </div>
    </div>
    <div id="tabNormaleCashOut" class="container-fluid tab-pane "><br>
        <div class="container table-responsive ">

        <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste Normale CashIn</b>
              </div>
            </div>
          </div>
          <div class="card-body">

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
      </div>
         
  </div>

  <div class="container-fluid tab-content">
  <div id="tabDat" class="container-fluid tab-pane "><br>   
    <div class="table-responsive">
    <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste Debit a Tort</b>
              </div>
            </div>
          </div>
          <div class="card-body">
      <table class="table table-bordered" id="tableDat">
        <thead style="text-align: center ;">
          <tr>
              <th colspan="12" style="text-align: center ;">BOA</th>
              <!-- <th style="text-align: center ;"></th>
              <th colspan="9" style="text-align: center ;">ORANGE</th>-->
              <th colspan="3">Régularisation</th> 
          </tr>
          <tr>
            <th><input type="checkbox" id="select-all"></th> 
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
            <th>Numéro</th>
            <th>Etat</th>
            <th>Date_regul</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>  
          <?php foreach($dat as $item) { ?> 
            <tr>
                    <td><input type="checkbox" name="selected_items[]" value="<?php echo $item->princ_ref_igor; ?>"></td>
                    <td><?php echo $item->comm_code_agence ?></td>
                    <td><?php echo $item->princ_date_oper ?></td>
                    <td><?php echo $item->princ_date_val ?></td>
                    <td><?php echo $item->comm_date_val ?></td>
                    <td><?php echo number_format($item->princ_montant, 0, ',', ' ') ?></td>
                    <td><?php echo number_format($item->comm_montant, 0, ',', ' ') ?></td>
                    <td><?php echo "" ?></td>
                    <td><?php echo $item->princ_devise?></td>
                    <td><?php echo $item->princ_oper ?></td>
                    <td><?php echo $item->princ_expl ?></td>
                    <td><?php echo $item->princ_ref_igor ?></td>
                    <td><?php echo substr($item->cle, 0, 10) ?></td>
                    <td>Non</td>
                    <td></td>
                    <td style="text-align: center;">
                      <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->princ_ref_igor ?>', 'Non', '')">
                        <i class="bx bx-edit" style="font-size: 20px;"></i>
                      </a> 
                      <a href="javascript:void(0);" class="text-primary" data-target="#myModalFr10"
                              data-compte="<?php echo $item->princ_compte ?>"
                              data-date-oper="<?php echo $item->princ_date_oper ?>"
                              data-montant="<?php echo $item->princ_montant ?>"
                              data-libelle="<?php echo $item->princ_libelle ?>"
                              data-oper="<?php echo $item->princ_oper ?>"
                              data-ref-igor="<?php echo $item->princ_ref_igor ?>" data-toggle="modal">
                        <i class="bx bx-printer" style="font-size: 20px;"></i>
                      </a>
                    </td>
              </tr>
          <?php } ?>  
          <input type="submit" class="btn btn-danger" name="delete_selected" value="Supprimer sélection" disabled>
          
        </tbody>
      </table>
    </div>
  </div>
    </div>
  </div>
  <div id="tabCat" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
    <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste Credit à tort</b>
              </div>
            </div>
          </div>
          <div class="card-body">
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
            <th>Numero</th>
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
              <td><?php echo number_format($item->princ_montant, 0, ",", " ") ?></td>
              <td><?php echo $item->princ_devise ?></td>
              <td><?php echo $item->princ_oper ?></td>
              <td><?php echo $item->princ_expl ?></td>
              <td><?php echo $item->princ_ref_igor ?></td>
              <td><?php echo substr($item->cle, 0, 10) ?></td>
              <td>Non</td>
              <td></td>
              <td style="text-align:center;">
                <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->princ_ref_igor ?>', 'Non', '')">
                  <i class="bx bx-edit"></i>
                </a>
                <a href="javascript:void(0);" class="text-primary" data-target="#myModalFr10"
                              data-compte="<?php echo $item->princ_compte ?>"
                              data-date-oper="<?php echo $item->princ_date_oper ?>"
                              data-montant="<?php echo $item->princ_montant ?>"
                              data-libelle="<?php echo $item->princ_libelle ?>"
                              data-oper="<?php echo $item->princ_oper ?>"
                              data-ref-igor="<?php echo $item->princ_ref_igor ?>" data-toggle="modal">
                <i class="bx bx-printer"></i>
                </a>
              </td>
            </tr>
            <input type="submit" class="btn btn-danger" name="delete_selected" value="Supprimer sélection">
        <?php } ?> 
        </tbody>
      </table>
        </div>
        </div>
    </div>     
  </div>

  <div id="tabMvts" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
    <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste MVTS</b>
              </div>
            </div>
          </div>
          <div class="card-body">
      <table class="table table-bordered" id="tableMvts">
        <thead>
          <tr>
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
    </div> 
    
    <div class="table-responsive">
    <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste MVTS</b>
              </div>
            </div>
          </div>
          <div class="card-body">
      <table class="table table-bordered" id="tableMvts">
        <thead>
          <tr>
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
    </div>
  </div>
  <div id="tabInd" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
    <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste IND</b>
              </div>
            </div>
          </div>
          <div class="card-body">
      <table class="table table-bordered" id="tableInd">
        <thead>
          <tr>
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
    </div>     
  </div>


  <div id="tabVI" class="container-fluid tab-pane"><br>
      <div class="table-responsive">
      <div class="card"> 
          <div class="card-header">
            <div class="row">
              <div class="col-lg-6">
                <b>Liste VI</b>
              </div>
            </div>
          </div>
          <div class="card-body">
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

<form id="modifForm">
  <div class="modal fade" id="myModal">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="card p-3">
          <div class="text-center" style="font-size: 17px;">
            <b>Régulariser</b>
            <button type="button" class="close" data-dismiss="modal">&times;</button>
          </div>
            <div class="row form-group mt-2">
                <div class="col-sm-6" style="display: none;">
                  <label for="ref">ref</label>
                  <input type="text" class="form-control" id="ref_igor" name="ref_igor">
                </div>
                <div class="col-sm-6">
                  <label for="etat">Etat</label>
                  <select name="etat" id="etat" class="form-control">
                    <option value="Non">Non</option>
                    <option value="En cours">En cours</option>
                  </select>
                </div>
                <div class="col-sm-6">
                  <label for="date">Date</label>
                  <input type="date" class="form-control" id="date_regul" name="date_regul" required>
                </div>
                </div>
                <div class="text-center">
                  <button type="button" class="btn btn-danger" data-dismiss="modal">Annuler</button>
                  <input type="submit" class="btn btn-success" value="Valider" />
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

document.addEventListener("DOMContentLoaded", function() {
      const selectAllCheckbox = document.getElementById("select-all");
      const dataCheckboxes = document.querySelectorAll("tbody input[type='checkbox']");

      selectAllCheckbox.addEventListener("change", function() {
        
        dataCheckboxes.forEach(function(checkbox) {
          checkbox.checked = selectAllCheckbox.checked;
        });
      });
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
      "&mobile_money=" + encodeURIComponent("ORANGE") +
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

  $("#tableMvts").DataTable({
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
