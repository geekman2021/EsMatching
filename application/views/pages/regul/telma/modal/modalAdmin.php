<?php //if (!empty($ci)) { ?>     
  <div class="modal fade" id="myModalAdmin" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
      <div class="modal-dialog" role="document">
          <div class="modal-content">
              <div class="modal-header">
                  <h6 class="text-center">Infos Deallocation</h6>
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                      <span aria-hidden="true">&times;</span>
                  </button>
              </div>
              <div class="modal-body">
                <div class="row">
                  <div class="col-sm-7">
                    <b>Régularisé</b>: 
                    <?php
                      $dateObj = new DateTime($ci[0]->date_regul);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le" .$formattedDate2 ?>
                    
                  </div>
                  <div class="col-sm-5">
                  <b>Réf: </b><?php echo $ci[0]->ref_igor; ?> </p>
                  </div>
                  <div class="col-sm-7">
                    <b>opération</b>: <?php echo $ci[0]->oper; ?> </p>
                  </div>
                  <div class="col-sm-5">
                  <b>Durée:</b>
                  <?php 
                      $date1 = new DateTime($ci[0]->DATE_OPER);
                      $date2 = new DateTime($ci[0]->date_regul);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jours";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                  <?php 
                      echo $ci[0]->montant ." MGA";
                    ?>
                  </div>
                  <div class="col-sm-5">
                  <b> Exploitant: </b>
                  <?php 
                      echo $ci[0]->expl;
                  ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
  </div>
<?php //} ?>