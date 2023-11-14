<?php if (!empty($ambi)) { ?>  
  <?php foreach($ambi as $item) { ?>   
  <div class="modal fade" id="myModalAmbigu<?= $item->id_ambiguous ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
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
                      $dateObj = new DateTime($item->rollback_transfer_date);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le" .$formattedDate2 ?>
                    
                  </div>
                  <div class="col-sm-5">
                  <b>Réference: </b><?php echo $item->rollback_reference_number; ?> </p>
                  </div>
                  <div class="col-sm-7">
                    <b>Service</b>: <?php echo $item->rollback_service_name; ?> </p>
                  </div>
                  <div class="col-sm-5">
                  <b>Durée:</b>
                  <?php 
                      $date1 = new DateTime($item->transfer_date);
                      $date2 = new DateTime($item->rollback_transfer_date);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jours";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                  <?php 
                      echo $item->amount ." MGA";
                    ?>
                  </div>
                  <div class="col-sm-5">
                  <b> Description: </b>
                  <?php 
                      echo $item->rollback_description;
                  ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
  </div>
<?php } ?>
<?php } ?>


<?php if (!empty($ambi_co)) { ?>  
  <?php foreach($ambi_co as $item) { ?>   
  <div class="modal fade" id="myModalAmbiguCO<?= $item->id_ambiguous ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
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
                      $dateObj = new DateTime($item->rollback_transfer_date);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le" .$formattedDate2 ?>
                    
                  </div>
                  <div class="col-sm-5">
                  <b>Réference: </b><?php echo $item->rollback_reference_number; ?> </p>
                  </div>
                  <div class="col-sm-7">
                    <b>Service</b>: <?php echo $item->rollback_service_name; ?> </p>
                  </div>
                  <div class="col-sm-5">
                  <b>Durée:</b>
                  <?php 
                      $date1 = new DateTime($item->transfer_date);
                      $date2 = new DateTime($item->rollback_transfer_date);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jours";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                  <?php 
                      echo $item->amount ." MGA";
                    ?>
                  </div>
                  <div class="col-sm-5">
                  <b> Description: </b>
                  <?php 
                      echo $item->rollback_description;
                  ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
  </div>
<?php } ?>
<?php } ?>