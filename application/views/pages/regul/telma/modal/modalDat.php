<?php if (!empty($ci)) { 
  foreach($ci as $item) {
?>  

  <div class="modal fade" id="myModalDat<?= $item->id_nonau ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
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
                      $dateObj = new DateTime($item->date_oper);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le " .$formattedDate2 ?>
                    
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
                      $date2 = new DateTime($item->date_oper);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jour";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                    <?= number_format($item->montant, 0, ',', ' ') ." MGA"; ?>
                  </div>
                  <div class="col-sm-5">
                  <b> Exploitant: </b>
                  <?= $item->expl; ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
  </div>
<?php } } ?>