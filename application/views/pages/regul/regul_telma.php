<div class=" container mt-5">
    <ul class="nav nav-tabs">
        <li class="nav-item col-sm-6">
            <a class="nav-link" data-toggle="tab" href="#tabBoa">BOA</a>
        </li>
        <li class="nav-item col-sm-6">
            <a class="nav-link" data-toggle="tab" href="#tabTelma">Telma</a>
        </li>
    </ul>
</div>


<div class="tab-content">
    <!-- BOA -->
    <div id="tabBoa" class="container mt-1 tab-pane "><br>  
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
        <div class="container-mt-5">
            <table class="table table-bordered" id="tableBoa">
                <thead style="text-align: center ;">
                    <tr>
                        <th>Compte</th>
                        <th>Date_Oper</th>
                        <th>Date_Val </th>
                        <th>Montant</th>
                        <th>Devise</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>Action</th>
                    </tr>
                </thead>
                <tbody>
                    <?php if(!empty($dat)) { ?>
                        <?php foreach($dat as $item) { ?>
                            <tr>
                                <td><?= $item->COMPTE ?></td>
                                <td><?= $item->DATE_OPER ?></td>
                                <td><?= $item->DATE_VAL ?> </td>
                                <td><?= $item->MONTANT ?></td>
                                <td><?= $item->DEVISE ?></td>
                                <td><?= $item->LIBELLE ?></td>
                                <td><?= $item->OPER ?></td>
                                <td><?= $item->EXPL ?></td>
                                <td><?= $item->REF_IGOR ?></td>
                                <td style="text-align: center;"><a href="#" class="btn btn-info" data-toggle="modal" data-target="#myModalBoa<?= $item->id_nonau ?>"><i class="bx bx-show" style="font-size: 20px;"> </i></a></td>
                            </tr>
                        
                        <?php } ?>
                    <?php } ?>

                </tbody>
            </table>
        </div>
           
    </div>

    
    <!-- Telma -->
    <div id="tabTelma" class="container mt-5 tab-pane ">
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
        <table id="tableTelma" class="table table-bordered">
            <thead>
            <tr> 
                <th>Transfer_id</th>
                <th>Date </th>
                <th>Heure</th>
                <th>Channel</th>
                <th>Type</th>
                <th>Etat</th>
                <th>Montant</th>
                <th>Envoyeur</th>
                <th>Receveur</th>
                <th>Action</th>
            </tr>
            </thead>
            <tbody>
            <?php //foreach($ci as $item)  { ?>
            <tr> 
                <td></td>
                <td></td>
                <td> </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td><a href="#" data-toggle="modal" data-target="#myModalTelma<?php //echo $item->id; ?>">Afficher Infos</a></td>
            </tr>
            <?php //} ?>
            </tbody>
        </table>
    </div>
</div>

<!-- Modal BOA -->

<?php if(!empty($dat)) { ?>
    <?php foreach($dat as $item) { ?>
    <div class="modal fade" id="myModalBoa<?= $item->id_nonau ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
      <div class="modal-dialog" role="document">
          <div class="modal-content">
              <div class="modal-header">
                  <h6 class="text-center">Infos</h6>
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                      <span aria-hidden="true">&times;</span>
                  </button>
              </div>
              <div class="modal-body">
                <div class="row">
                  <div class="col-sm-7">
                    <b>Régularisé</b>: 
                    <?php 
                      $dateObj = new DateTime($item->date_regul);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le" .$formattedDate2 ?>
                    
                  </div>
                  <div class="col-sm-5">
                  <b>Réf: </b><?php echo $item->ref_igor; ?> </p>
                  </div>
                  <div class="col-sm-7">
                    <b>opération</b>: <?php echo $item->oper; ?> </p>
                  </div>
                  <div class="col-sm-5">
                  <b>Durée:</b>
                  <?php 
                      $date1 = new DateTime($item->DATE_OPER);
                      $date2 = new DateTime($item->date_regul);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jours";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                  <?php 
                      echo $item->montant ." MGA";
                    ?>
                  </div>
                  <div class="col-sm-5">
                  <b> Exploitant: </b>
                  <?php 
                      echo $item->expl;
                  ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
    </div>
  <?php } ?>
<?php } ?>

<script>
    let tableBoa = new DataTable('#tableBoa', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
        }
    });
</script>




