<?php if (!empty($vi_deallo)) { ?>
<?php foreach($vi_deallo as $item) { ?>     
  <div class="modal fade" id="myModalDeallo<?= $item->id_deallo ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
      <div class="modal-dialog" role="document">
          <div class="modal-content">
              <div class="modal-header">
                  <h6 class="text-center">Infos VI</h6>
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                      <span aria-hidden="true">&times;</span>
                  </button>
              </div>
              <div class="modal-body">
                <div class="row">
                  <div class="col-sm-7">
                    <b>Régularisé</b>: 
                    <?php
                      $dateObj = new DateTime($item->DATE_OPER);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le " .$formattedDate2 ?>
                    
                  </div>
                  <div class="col-sm-5">
                  <b>Réf: </b><?php echo $item->REF_IGOR; ?> </p>
                  </div>
                  <div class="col-sm-7">
                    <b>opération</b>: <?php echo $item->OPER; ?> </p>
                  </div>
                  <div class="col-sm-5">
                  <b>Durée:</b>
                  <?php 
                      $date1 = new DateTime($item->DATE_OPER);
                      $date2 = new DateTime($item->transfer_date);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jour";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                  <?= number_format($item->MONTANT, 0, ',', ' ') ." MGA";?>
                  </div>
                  <div class="col-sm-5">
                  <b> Exploitant: </b>
                  <?php 
                      echo $item->EXPL;
                  ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
  </div>
<?php } ?>
<?php } ?>