
<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>

<div class=" container-fluid mt-5">

  
    <ul class="nav nav-tabs">
            <li class="nav-item col-sm-6">
                <a class="nav-link" href="#normale">Normale</a>
            </li>
            <li class="nav-item col-sm-6">
                <a class="nav-link" href="#anormale">Anomalie</a>
            </li>
    </ul>
</div>

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
          <a class="nav-link" href="#tabMvts">MVTS</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabAdmin">Admin</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabVI">VI</a>
        </li>
      </ul>
    </div>

  </div>
  <div class="container-fluid tab-content">
    <div class="row">
      
    </div>
    
    <div id="tabNormaleCashIn" class="tab-pane"><br>
        <div class="row">
          <div class="col-sm-6">
              <label for="dateDebut">Date Debut</label>
              <input type="text" class="form-control" id="min" name="min">
          </div>
          <div class="col-sm-6">
            <label for="dateDebut">Date Fin</label>
              <input type="text" class="form-control" id="max" name="max">
          </div> 
        </div>
            <div class="table-responsive mt-1">
            <div class="card">
      <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste Normale CASH IN</b>
            </div>
          </div>
      </div>
      <div class="card-body">
                <table class="table table-bordered" id="tableNormaleCashIn">
                    <thead style="text-align: center ;">
                        <tr>
                            <th colspan="9" style="text-align: center ;">BOA</th>
                            <th></th>
                            <th colspan="10" style="text-align: center ;">Telma</th>
                        </tr>
                        <tr> 
                            <th>Date_Oper</th>
                            <th>Date_Val</th>
                            <th>Heure</th>
                            <th>Montant</th>
                            <th>Devise</th>
                            <th>Libelle</th>
                            <th>Operation</th>
                            <th>Expl</th>
                            <th>ReferenceIgor</th>
                            <th>Numero</th>
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
                        <tr>
                          <td><?php echo $item->DATE_OPER ?></td>
                          <td><?php echo $item->DATE_VAL ?></td>
                          <td><?php echo $item->BOA_HEURE ?></td>
                          <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
                          <td><?php echo $item->DEVISE ?></td>
                          <td><?php echo $item->LIBELLE ?></td>
                          <td><?php echo $item->OPER ?></td>
                          <td><?php echo $item->EXPL ?></td>
                          <td><?php echo $item->REF_IGOR ?></td>
                          <td><?php echo $item->cle ?></td>
                          <td><?php echo $item->date ?></td>
                          <td><?php echo $item->telma_heure ?></td>
                          <td><?php echo $item->trans_id ?></td>
                          <td><?php echo $item->TYPE ?></td>
                          <td><?php echo $item->channel ?></td>
                          <td><?php echo $item->state ?></td>
                          <td><?php echo number_format($item->Amount_MGA, 0, ',', " ") ?></td>
                          <td><?php echo $item->sender_name ?></td>
                          <td><?php echo $item->receiver_name ?></td>
                          
                        </tr>

                      <?php }?>

                        
                        
                    </tbody>
                </table>
            </div>
        </div>
                      </div>
                      </div>



        <div id="tabNormaleCashOut" class="container-fluid tab-pane "><br>
        
            <div class="table-responsive">
            <div class="card">
      <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste Normale CASH Out</b>
            </div>
          </div>
      </div>
      <div class="card-body">
                <table class="table table-bordered" id="tableNormaleCashOut">
                <thead style="text-align: center ;">
                        <tr>
                            <th colspan="9" style="text-align: center ;">BOA</th>
                            <th></th>
                            <th colspan="10" style="text-align: center ;">Telma</th>
                        </tr>
                        <tr>
                            
                          <th>Date_Oper</th>
                          <th>Date_Val</th>
                          <th>Heure</th>
                          <th>Montant</th>
                          <th>Devise</th>
                          <th>Libelle</th>
                          <th>Operation</th>
                          <th>Expl</th>
                          <th>ReferenceIgor</th>
                          <th>Numero</th>
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
                      <?php foreach($normal_co as $item) { ?>
                        <tr>
                        <td><?php echo $item->DATE_OPER ?></td>
                          <td><?php echo $item->DATE_VAL ?></td>
                          <td><?php echo $item->BOA_HEURE ?></td>
                          <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
                          <td><?php echo $item->DEVISE ?></td>
                          <td><?php echo $item->LIBELLE ?></td>
                          <td><?php echo $item->OPER ?></td>
                          <td><?php echo $item->EXPL ?></td>
                          <td><?php echo $item->REF_IGOR ?></td>
                          <td><?php echo substr($item->cle, 0, 10) ?></td>
                          <td><?php echo $item->date ?></td>
                          <td><?php echo $item->telma_heure ?></td>
                          <td><?php echo $item->trans_id ?></td>
                          <td><?php echo $item->TYPE ?></td>
                          <td><?php echo $item->channel ?></td>
                          <td><?php echo $item->state ?></td>
                          <td><?php echo number_format($item->Amount_MGA, 0, ',', " ") ?></td>
                          <td><?php echo $item->sender_name ?></td>
                          <td><?php echo $item->receiver_name ?></td>

                      <?php }?>

                        </tr>
                        
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
              <b>Liste Débit à tort</b>
            </div>
          </div>
      </div>
      <div class="card-body">
      <table class="table table-bordered" id="tableDat">
      <thead style="text-align: center ;">
        <tr>
            <th colspan="9" style="text-align: center ;">BOA</th>
            <th colspan="3" style="text-align: center ;">Régularisation</th>
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
            <th>Etat</th>
            <th>Date_Regul</th>
            <th>Action</th>
        </tr>
        </thead>
        <tbody>
        <?php foreach($dat as $item) { ?>
          <tr>
          <td><?php echo $item->COMPTE ?></td>
            <td><?php echo $item->DATE_OPER ?></td>
            <td><?php echo $item->DATE_VAL ?></td>
            <td><?php echo $item->DEVISE ?></td>
            <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
            <td><?php echo $item->LIBELLE ?></td>
            <td><?php echo $item->OPER ?></td>
            <td><?php echo $item->EXPL ?></td>
            <td><?php echo $item->REF_IGOR ?></td>
            <td><?php echo $item->etat ?></td>
            <td><?php echo "" ?></td>
            <td style="text-align-center;">
              <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->REF_IGOR ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
              <i class="bx bx-edit" style="font-size: 20px;"></i>
              </a> 
              <a href="javascript:void(0);" class="text-primary" data-target="#myModalFr10"
                    data-compte="<?php echo $item->COMPTE ?>"
                    data-date-oper="<?php echo $item->DATE_OPER ?>"
                    data-montant="<?php echo $item->MONTANT ?>"
                    data-libelle="<?php echo $item->LIBELLE ?>"
                    data-oper="<?php echo $item->OPER ?>"
                    data-ref-igor="<?php echo $item->REF_IGOR ?>" data-toggle="modal">
                    <i class="bx bx-printer" style="font-size: 20px;"></i>
                </a>
            </td>
          </tr>
        <?php }?>
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
              <b>Liste Credit à Tort</b>
            </div>
          </div>
      </div>
      <div class="card-body">
      <table class="table table-bordered" id="tableCat">
      <thead style="text-align: center ;">
        <tr>
            <th colspan="8" style="text-align: center ;">BOA</th>
            <th colspan="3" style="text-align: center ;">Régularisation</th>
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
            <th>Etat</th>
            <th>Date_Regul</th>
            <th>Action</th>
        </tr>
        </thead>
        <tbody>
        <?php foreach($cat as $item) { ?>
          <tr>
            <td><?php echo $item->DATE_OPER ?></td>
            <td><?php echo $item->DATE_VAL ?></td>
            <td><?php echo $item->DEVISE ?></td>
            <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
            <td><?php echo $item->LIBELLE ?></td>
            <td><?php echo $item->OPER ?></td>
            <td><?php echo $item->EXPL ?></td>
            <td><?php echo $item->REF_IGOR ?></td>
            <td><?php echo "" ?></td>
            <td><?php echo "" ?></td>
            <td><?php echo "" ?></td>
          </tr>

        <?php }?>
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
            <th style="display: none;"></th>
            <th>Date</th>
            <!-- <th>Heure</th> -->
            <th>Transfer_id</th>
            <th>Type</th>
            <th>Channel</th>
            <th>Etat</th>
            <th>Montant</th>
            <th>sender</th>
            <th>receiver</th>
            <th>Etat</th>
            <th>Date_Regul</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>
        <!-- <?php //foreach($admin as $item) { ?>
          <tr>
            <td style="display: none;"><?php echo $item->id ?></td>
            <td><?php echo $item->date ?></td>
            <td><?php //echo $item->telma_heure ?></td>
            <td><?php //echo $item->trans_id ?></td>
            <td><?php //echo $item->TYPE ?></td>
            <td><?php //echo $item->channel ?></td>
            <td><?php //echo $item->state ?></td>
            <td><?php //echo number_format($item->Amount_MGA, 0, ',', " ") ?></td>
            <td><?php //echo $item->sender ?></td>
            <td><?php //echo $item->receiver ?></td>
            <td><?php //echo $item->etat ?></td>
            <td><?php //echo $item->date_regul ?></td>
            <td>
              <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->id ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                Modifier
              </a> 
            </td>
          </tr>

        <?php //}?> -->
        </tbody>
      </table>
    </div>     
  </div>
        </div>
        </div>

  <div id="tabAdmin" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
    <div class="card">
      <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste Admin</b>
            </div>
          </div>
      </div>
      <div class="card-body">
      <table class="table table-bordered" id="tableAdmin">
        <thead>
          <tr>
            <th style="display: none;"></th>
            <th>Date</th>
            <!-- <th>Heure</th> -->
            <th>Transfer_id</th>
            <th>Type</th>
            <th>Channel</th>
            <th>Etat</th>
            <th>Montant</th>
            <th>sender</th>
            <th>receiver</th>
            <th>Etat</th>
            <th>Date_Regul</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($admin as $item) { ?>
          <tr>
            <td style="display: none;"><?php echo $item->id ?></td>
            <td><?php echo $item->date ?></td>
            <!-- <td><?php //echo $item->telma_heure ?></td> -->
            <td><?php echo $item->trans_id ?></td>
            <td><?php echo $item->TYPE ?></td>
            <td><?php echo $item->channel ?></td>
            <td><?php echo $item->state ?></td>
            <td><?php echo number_format($item->Amount_MGA, 0, ',', " ") ?></td>
            <td><?php echo $item->sender ?></td>
            <td><?php echo $item->receiver ?></td>
            <td><?php echo $item->etat ?></td>
            <td><?php echo $item->date_regul ?></td>
            <td>
              <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->id ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                Modifier
              </a> 
            </td>
          </tr>

        <?php }?>
        </tbody>
      </table>
    </div>     
  </div>
        </div>
        </div>

  <div id="tabVI" class="tab-pane"><br>
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
                    <th>DateOper</th>
                    <th>DateVal</th>
                    <th>Montant</th>
                    <th>Devise</th>
                    <th>ReferenceIgor</th>
                    <th>Libelle</th>
                    <th>Operation</th>
                    <th>Expl</th>
                    <th>Etat</th>
                    <th>Date_Regul</th>
                    <th>Action</th>
                  </tr>
              </thead>
              <tbody>
                <?php foreach($vi as $item) { ?>
                  <tr>
                    <td><?php echo $item->DATE_OPER ?></td>
                    <td><?php echo $item->DATE_VAL ?></td>
                    <td><?php echo number_format($item->MONTANT, 0, ',', " ") ?></td>
                    <td><?php echo $item->DEVISE ?></td>
                    <td><?php echo $item->REF_IGOR ?></td>
                    <td><?php echo $item->LIBELLE ?></td>
                    <td ><?php echo $item->OPER ?></td>
                    <td><?php echo $item->EXPL ?></td>
                    <td><?php echo $item->etat ?></td>
                    <td><?php echo $item->date_regul ?></td>
                    <td>
                      <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->id ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                        <box-icon name='printer'  ></box-icon>Modifier
                      </a> 
                    </td>
                  </tr>
                <?php }?>
              </tbody>
          </table>
      </div>
                </div>
                </div>
  </div>
</div>
</div>


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
                  <input type="text" id="compte" name="compte"  pattern=".{11,}" title="Le compte doit avoir au moins 11 caractères" maxlength="11" class="form-control" required>
                </div>
                <div class="col-sm-12">
                  <label for="comm">Compte commission</label>
                  <input type="text" id="comm" name="comm" class="form-control"  pattern=".{11,}" title="Le compte doit avoir au moins 11 caractères" maxlength="11" required>
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

function modifier(ref_igor, etat, date) {
    $("#ref_igor").val(ref_igor);
    $("#etat").val(etat);
    $("#date_regul").val(date);
  }


$(document).ready(function(){

  var compteInput = $('#compte');
  var commInput = $('#comm');

        // Fonction pour afficher le message d'erreur
        function afficherMessageErreur(input) {
            var messageErreur = input.attr('title');
            input.next('span').remove(); // Supprimez tout message d'erreur précédent
            input.after('<span class="text-danger">' + messageErreur + '</span>');
        }

        // Écoutez l'événement de changement d'entrée
        compteInput.on('input', function () {
            if (compteInput.val().length < 11) {
                afficherMessageErreur(compteInput);
            } else {
                compteInput.next('span').remove();
            }
        });

        commInput.on('input', function () {
            if (commInput.val().length < 11) {
                afficherMessageErreur(commInput);
            } else {
                commInput.next('span').remove();
            }
        });

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

    // $('#tableNormaleCashIn').DataTable( {
    //     dom: 'Bfrtip',
    //     responsive: true,
    //     scrollX: true,
    //     buttons: [
    //         'copy', 'csv', 'excel', 'pdf', 'print'
    //         ],
    //     language: {
    //       url: '<?php echo base_url(); ?>assets/fr-FR.json',
    // }
    // });

  $("#tableDat").DataTable({
    responsive: true,
    "scrollX": true,
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
    "scrollX": true,

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
    "scrollX": true,

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
    "scrollX": true,

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
    "scrollX": true,

    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });
  


    $("#tableAnormaleCashOut").DataTable();
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
<script>
  let minDate, maxDate;
 DataTable.ext.search.push(function (settings, data, dataIndex) {
     let min = minDate.val();
     let max = maxDate.val();
     let date = new Date(data[0]);
  
     if (
         (min === null && max === null) ||
         (min === null && date <= max) ||
         (min <= date && max === null) ||
         (min <= date && date <= max)
     ) {
         return true;
     }
     return false;
 });
  
 // Create date inputs
 minDate = new DateTime('#min', {
    format: 'LL', 
    locale: 'fr', 
});

maxDate = new DateTime('#max', {
    format: 'LL', 
    locale: 'fr', 
});
  
 // DataTables initialisation
 let table = new DataTable('#tableNormaleCashIn', {
  scrollX: true,
  language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
 }
 );
  

 document.querySelectorAll('#min, #max').forEach((el) => {
     el.addEventListener('change', () => table.draw());
 });
 

</script>

<script>
   function ouvrirEtImprimer(date_oper, ref_igor, montant, oper, compte) {

    var url = '<?php echo site_url('Imprimer_Telma/imprimerDocument'); ?>' +
      '?date=' + encodeURIComponent(date_oper) +
      '&ref=' + encodeURIComponent(ref_igor) +
      '&oper=' + encodeURIComponent(oper)+
      '&montant=' + encodeURIComponent(montant)+
      '&compte=' + encodeURIComponent(compte);
      
    var nouvelleFenetre = window.open(url, '_blank');
    

}

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
      "&operateur="  + encodeURIComponent("MVL") +
      "&mobile_money=" + encodeURIComponent("M VOLA") +
      "&tax="  + encodeURIComponent("333,33") +
      "&tva=" + encodeURIComponent("66,67") +
      "&montant_comm=" + encodeURIComponent("400,00") +
      "&place=" + encodeURIComponent(place)
    var nouvelleFenetre = window.open(url, '_blank'); 
    $('#myModalFr10').modal('hide');
    $("#compte").val("");
    $("#comm").val("");
    $("#nom").val("");


});

</script>
</script>
