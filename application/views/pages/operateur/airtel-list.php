<div class="container-fluid mt-5">
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
    <div id="normale" class="container-fluid tab-pane"><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashIn">Normale Cash in </a>
        </li>
        <li class="nav-item col-sm-6">
          <a class="nav-link" href="#tabNormaleCashOut">Normale Cash Out</a>
        </li>
      </ul>
    </div>
    <div id="anormale" class="container-fluid tab-pane fade"><br>
      <ul class="nav nav-tabs">
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabDat">Débit à Tort</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabCat">Crédit à Tort </a>
        </li>
        <li class="nav-item col-sm-1">
          <a class="nav-link" href="#tabMvts">MVTS</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabAmbi">Ambigu</a>
        </li>
        <li class="nav-item col-sm-2">
          <a class="nav-link" href="#tabDeallo">Dealloc</a>
        </li>
        <li class="nav-item col-sm-1">
          <a class="nav-link" href="#tabVI">VI</a>
        </li>
      </ul>
    </div>

  </div>
  <div class="container-fluid tab-content">
    <div id="tabNormaleCashIn" class="tab-pane"><br>
        <div class="table-responsive">
            <table class="table table-bordered" id="tableNormaleCashIn">
                <thead style="text-align: center ;">
                    <tr>
                        <th colspan="9" style="text-align: center ;">BOA</th>
                        <th colspan="10" style="text-align: center ;">Airtel</th>
                    </tr>
                    <tr>
                        <th>Compte</th>
                        <th>DateOper</th>
                        <th>DateVal</th>
                        <th>Devise</th>
                        <th>Montant</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>ExternalId</th>
                        <th>TransferId</th>
                        <th>TransferDate</th>
                        <th>AccountNo</th>
                        <th>SenderMsiSDN</th>
                        <th>DestMsiSDN</th>
                        <th>Amount</th>
                        <th>Description</th>
                        <th>ServiceName</th>
                        <th>ReferenceNumber</th>
                    </tr>
                </thead>
                <tbody>
                  <?php foreach($normale_ci as $item) { ?>
                    <tr>
                      <td><?php echo $item->COMPTE ?></td>
                      <td><?php echo $item->DATE_OPER ?></td>
                      <td><?php echo $item->DATE_VAL ?></td>
                      <td><?php echo $item->DEVISE ?></td>
                      <td><?php echo number_format($item->MONTANT, 0, ',', " ")  ?></td>
                      <td><?php echo $item->LIBELLE ?></td>
                      <td><?php echo $item->OPER ?></td>
                      <td><?php echo $item->EXPL ?></td>
                      <td><?php echo $item->REF_IGOR ?></td>
                      <td><?php echo $item->external_id ?></td>
                      <td><?php echo $item->TRANSFER_ID ?></td>
                      <td><?php echo $item->transfer_date ?></td>
                      <td><?php echo $item->account_no ?></td>
                      <td><?php echo $item->sender_msisdn ?></td>
                      <td><?php echo $item->dest_msisdn ?></td>
                      <td><?php echo number_format($item->amount, 0, ',', " ")  ?></td>
                      <td><?php echo $item->description ?></td>
                      <td><?php echo $item->service_name ?></td>
                      <td><?php echo $item->reference_number ?></td>
                    </tr>

                  <?php } ?>
                </tbody>
            </table>
        </div>
    </div>
    <div id="tabNormaleCashOut" class="container-fluid tab-pane "><br>
        <div class="table-responsive">
            <table class="table table-bordered" id="tableNormaleCashOut">
                <thead>
                    <tr>
                        <th colspan="9" style="text-align: center ;">BOA</th>
                        <th colspan="10" style="text-align: center ;">AIRTEL</th>
                    </tr>
                    <tr>
                      <th>Compte</th>
                      <th>DateOper</th>
                      <th>DateVal</th>
                      <th>Devise</th>
                      <th>Montant</th>
                      <th>Libelle</th>
                      <th>Operation</th>
                      <th>Expl</th>
                      <th>ReferenceIgor</th>
                      <th>ExternalId</th>
                      <th>TransferId</th>
                      <th>TransferDate</th>
                      <th>AccountNo</th>
                      <th>SenderMsiSDN</th>
                      <th>DestMsiSDN</th>
                      <th>Amount</th>
                      <th>Description</th>
                      <th>ServiceName</th>
                      <th>ReferenceNumber</th>
                    </tr>
                </thead>
                <tbody>
                  <?php foreach($normale_co as $item) { ?>
                    <tr>
                      <td><?php echo $item->COMPTE ?></td>
                      <td><?php echo $item->DATE_OPER ?></td>
                      <td><?php echo $item->DATE_VAL ?></td>
                      <td><?php echo $item->DEVISE ?></td>
                      <td><?php echo number_format($item->MONTANT, 0, ',', " ")  ?></td>
                      <td><?php echo $item->LIBELLE ?></td>
                      <td><?php echo $item->OPER ?></td>
                      <td><?php echo $item->EXPL ?></td>
                      <td><?php echo $item->REF_IGOR ?></td>
                      <td><?php echo $item->external_id ?></td>
                      <td><?php echo $item->TRANSFER_ID ?></td>
                      <td><?php echo $item->transfer_date ?></td>
                      <td><?php echo $item->account_no ?></td>
                      <td><?php echo $item->sender_msisdn ?></td>
                      <td><?php echo $item->dest_msisdn ?></td>
                      <td><?php echo number_format($item->amount, 0, ',', " ")  ?></td>
                      <td><?php echo $item->description ?></td>
                      <td><?php echo $item->service_name ?></td>
                      <td><?php echo $item->reference_number ?></td>
                    </tr>

                  <?php } ?>
                 
                </tbody>
            </table>
        </div>
         
  </div>

<div class="container-fluid tab-content">
  <div id="tabDat" class="container-fluid tab-pane "><br>   
    <div class="table-responsive">
      <table class="table table-bordered" id="tableDat" style="width: 100%">
        <thead>
            <tr>
              <th style="display: none;">Id</th>
              <th>Compte</th>
              <th>Date_Oper</th>
              <th>Date_Val </th>
              <th>Devise</th>
              <th>Montant</th>
              <th>Libelle</th>
              <th>Operation</th>
              <th>Expl</th>
              <th>ReferenceIgor</th>
              <th>Action</th>
            </tr>
        </thead>
        <tbody>
          <?php foreach($igor_anomalie_ci as $item) { ?>
              <tr>
                <td style="display: none;"><?= $item->id ?></td>
                <td><?= $item->COMPTE ?></td>
                <td><?= $item->DATE_OPER ?></td>
                <td><?= $item->DATE_VAL ?></td>
                <td><?= $item->DEVISE ?></td>
                <td><?= number_format($item->MONTANT, 0, ',', " ")  ?></td>
                <td><?= $item->LIBELLE ?></td>
                <td><?= $item->OPER ?></td>
                <td><?= $item->EXPL ?></td>
                <td><?= $item->REF_IGOR ?></td>
                <td style= "text-align: center;">
                <!-- <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal" onClick="modifier('<?php echo $item->REF_IGOR ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                <i class="bx bx-edit" style="font-size: 25px;"></i>
                </a>  -->
                <a href="javascript:void(0);" class="text-primary" data-target="#myModalFr10"
                    data-compte="<?php echo $item->COMPTE ?>"
                    data-date-oper="<?php echo $item->DATE_OPER ?>"
                    data-montant="<?php echo $item->MONTANT ?>"
                    data-libelle="<?php echo $item->LIBELLE ?>"
                    data-oper="<?php echo $item->OPER ?>"
                    data-ref-igor="<?php echo $item->REF_IGOR ?>" data-toggle="modal"><i class="bx bx-printer" style="font-size: 25px;"></i>
                </a>
              </td>
            </tr>
          <?php } ?>
          
        </tbody>
      </table>
    </div>
  </div>
  <div id="tabCat" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableCat">
      <thead style="text-align: center ;">
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
              <!-- <th>Action</th> -->
            </tr>
        </thead>
        <tbody>
          <?php foreach($igor_anomalie_co as $item) { ?>
              <tr>
                <td><?php echo $item->COMPTE ?></td>
                <td><?php echo $item->DATE_OPER ?></td>
                <td><?php echo $item->DATE_VAL ?></td>
                <td><?php echo $item->DEVISE ?></td>
                <td><?php echo  number_format($item->MONTANT, 0, ',', " ") ?></td>
               
                <td><?php echo $item->LIBELLE ?></td>
                <td><?php echo $item->OPER ?></td>
                <td><?php echo $item->EXPL ?></td>
                <td><?php echo $item->REF_IGOR ?></td>
                <!-- <td>
                <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal2" onClick="modifier2('<?php echo $item->REF_IGOR ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                <i class="bx bx-edit" style="font-size: 25px;"></i>
                </a> 
              </td> -->
              </tr>
          <?php } ?>
          <?php //foreach($anomalie_co as $item) { ?>
            <!-- <tr> -->
              <!-- <td><?php //echo  $item->external_id ?></td>
              <td><?php// echo $item->TRANSFER_ID ?></td>
              <td><?php// echo $item->transfer_date ?></td>
              <td><?php //echo $item->account_no ?></td>
              <td><?php //echo $item->sender_msisdn ?></td>
              <td><?php //echo $item->dest_msisdn ?></td>
              <td><?php //echo $item->amount ?></td>
              <td><?php //echo $item->description ?></td>
              <td><?php //echo $item->service_name ?></td>
              <td><?php //echo $item->reference_number ?></td>
              <td></td>
              <td></td>
              <td></td> -->
            <!-- </tr> -->
          <?php //} ?>
        </tbody>
      </table>
    </div>     
  </div>

  <div id="tabAmbi" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
    <div class="card">
      <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste Ambigu CASH IN</b>
            </div>
          </div>
      </div>
      <div class="card-body">
      <table class="table table-bordered" id="tableAmbi">
        <thead>
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
          </tr>
        </thead>
        <tbody>
          <?php foreach ($ambiguous as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo number_format($item->amount, 0, ',', " ") ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
          <?php } ?>

        </tbody>
      </table>
    </div>
    </div>
  </div>
    <div class="table-responsive mt-5">
      <div class="card">
        <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste Ambigu Cash Out</b>
            </div>
          </div>
        </div>
        <div class="card-body">
        <table class="table table-bordered" id="tableAmbiCO">
        <thead>
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
          </tr>
        </thead>
        <tbody>
          <?php foreach ($ambiguousCO as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo number_format($item->amount, 0, ',', " ") ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
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
              <b>Liste MVTS CASH IN</b>
            </div>
          </div>
      </div>
      <div class="card-body">
      <table class="table table-bordered" id="tableMvtsCI">
        <thead>
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
              <!-- <th>Action</th> -->
          </tr>
        </thead>
        <tbody>
          <?php foreach ($anomalie_ci as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo number_format($item->amount, 0, ',', " ") ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
          <?php } ?>

        </tbody>
      </table>
    </div>
    </div>
  </div>
    <div class="table-responsive mt-5">
      <div class="card">
        <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste MVTS Cash Out</b>
            </div>
          </div>
        </div>
        <div class="card-body">
        <table class="table table-bordered" id="tableMvtsCO">
          <thead>
          <tr>
              <th style="display: none;">Id</th>
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
              <th>Action</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach($anomalie_co as $item) { ?>
            <tr>
              <td style="display: none;"><?php echo $item->id ?></td>
              <td><?=  $item->external_id ?></td>
              <td><?=  $item->TRANSFER_ID ?></td>
              <td><?=  $item->transfer_date ?></td>
              <td><?=  $item->account_no ?></td>
              <td><?=  $item->sender_msisdn ?></td>
              <td><?=  $item->dest_msisdn ?></td>
              <td><?=  $item->amount ?></td>
              <td><?=  $item->description ?></td>
              <td><?=  $item->service_name ?></td>
              <td><?=  $item->reference_number ?></td>
              <td>
                <a href="javascript:void(0);" class="text-warning" data-toggle="modal" data-target="#myModal2" onClick="modifier2('<?php echo $item->external_id ?>', '<?php echo $item->etat ?>', '<?php echo $item->date_regul ?>')">
                  <box-icon name='printer'  ></box-icon>Modifier
                </a>
                <!-- <a href="javascript:void(0);" class="text-primary" data-target="#myModalFr10"
                    data-compte="<?php //echo $item->COMPTE ?>"
                    data-date-oper="<?php //echo $item->DATE_OPER ?>"
                    data-montant="<?php //echo $item->MONTANT ?>"
                    data-libelle="<?php //echo $item->LIBELLE ?>"
                    data-oper="<?php //echo $item->OPER ?>"
                    data-ref-igor="<?php //echo $item->REF_IGOR ?>" data-toggle="modal">
                  <box-icon name="printer"></box-icon>Imprimer
                </a> -->
              </td>
            </tr>
            <?php } ?>

        </tbody>
      </table> 
        </div>
      </div>
      
    </div>     
  </div>

<div id="tabSucces" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
    <div class="card">
      <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste Succès CASH IN</b>
            </div>
          </div>
      </div>
      <div class="card-body">
      <table class="table table-bordered" id="tableSuccesCI">
        <thead>
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
          </tr>
        </thead>
        <tbody>
          <?php foreach ($success_no_refCI as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo number_format($item->amount, 0, ',', " ") ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
          <?php } ?>

        </tbody>
      </table>
    </div>
    </div>
  </div>
    <div class="table-responsive mt-5">
      <div class="card">
        <div class="card-header">
          <div class="row">
            <div class="col-lg-6">
              <b>Liste Succès Cash Out</b>
            </div>
          </div>
        </div>
        <div class="card-body">
        <table class="table table-bordered" id="tableSuccesCO">
        <thead>
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
          </tr>
        </thead>
        <tbody>
          <?php foreach ($success_no_refCO as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo number_format($item->amount, 0, ',', " ") ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
          <?php } ?>

        </tbody>
      </table> 
        </div>
      </div>
      
    </div>     
  </div>
 

  <div id="tabDeallo" class="container-fluid tab-pane "><br>    
    <div class="table-responsive">
      <table class="table table-bordered" id="tableDeallo">
      <thead>
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
          </tr>
        </thead>
        <tbody>
          <?php foreach ($deallocation as $item) { ?>
          <tr>
            <td><?php echo $item->TRANSFER_ID ?></td>
            <td><?php echo $item->transfer_date ?></td>
            <td><?php echo $item->external_id ?></td>
            <td><?php echo $item->account_no ?></td>
            <td><?php echo $item->sender_msisdn ?></td>
            <td><?php echo $item->dest_msisdn ?></td>
            <td><?php echo number_format($item->amount, 0, ',', " ")  ?></td>
            <td><?php echo $item->description ?></td>
            <td><?php echo $item->service_name ?></td>
            <td><?php echo $item->reference_number ?></td>
          </tr>   
          <?php } ?>

        </tbody>
      </table>
    </div>     
  </div>
  <div id="tabVI" class="container-fluid tab-pane "><br>   
    <div class="table-responsive">
      <table class="table table-bordered" id="tableVI">
        <thead style="text-align: center ;">
            <tr>
              <th>Compte</th>
              <th>Date_Oper</th>
              <th>Date_Val </th>
              <th>Devise</th>
              <th>Montant</th>
              <th>Libelle</th>
              <th>Operation</th>
              <th>Expl</th>
              <th>ReferenceIgor</th>
            </tr>
        </thead>
        <tbody>
          <?php foreach($vi as $item) { ?>
              <tr>
                <td><?php echo $item->COMPTE ?></td>
                <td><?php echo $item->DATE_OPER ?></td>
                <td><?php echo $item->DATE_VAL ?></td>
                <td><?php echo $item->DEVISE ?></td>
                <td><?php echo number_format($item->MONTANT, 0, ',', " ")  ?></td>
                <td><?php echo $item->LIBELLE ?></td>
                <td><?php echo $item->OPER ?></td>
                <td><?php echo $item->EXPL ?></td>
                <td><?php echo $item->REF_IGOR ?></td>
            </tr>
          <?php } ?>
    
        </tbody>
      </table>
    </div>
  </div>
</div>
</div>

<!-- Modal  -->
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

                  

        <!-- En-tête de la modal -->
        <!-- <div class="modal-header">
          <h4 class="modal-title">Regularisation</h4>
          <button type="button" class="close" data-dismiss="modal">&times;</button>
        </div> -->

       
        <!-- <form action="modifForm">

          <div class="modal-body">

            <div class="row">
              <div class="col-sm-6" style="display: none;">
                <label for="id">Id</label>
                <input type="text" id="id" name="id">
              </div>
              <div class="col-sm-6">
                <label for="etat">Etat</label>
                <select name="etat" id="etat">
                  <option value="Non">Non</option>
                  <option value="En cours">En cours</option>
                </select>
              </div>
              <div class="col-sm-6">
                <label for="date">Date</label>
                <input type="date" id="date_regul" name="date_regul">
              </div>
              </div>
            </div>
            
          </div>


          <div class="modal-footer">
            <button type="button" class="btn btn-danger" data-dismiss="modal">Annuler</button>
            <input type="submit" class="btn btn-success">Valider</button>
          </div>
        </form> -->


      </div>
    </div>
  </div>
</form>


<form id="modifForm2">
  <div class="modal fade" id="myModal2">
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
                  <input type="text" class="form-control" id="ref_igor2" name="ref_igor2">
                </div>
                <div class="col-sm-6">
                  <label for="etat">Etat</label>
                  <select name="etat2" id="etat2" class="form-control">
                    <option value="Non">Non</option>
                    <option value="En cours">En cours</option>
                  </select>
                </div>
                <div class="col-sm-6">
                  <label for="date">Date</label>
                  <input type="date" class="form-control" id="date_regul2" name="date_regul2" required>
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
                  <input type="text" id="compte" name="compte" class="form-control" maxlength="11" pattern=".{11,}" title="Le compte doit avoir au moins 11 caractères" required>
                </div>
                <div class="col-sm-12">
                  <label for="comm">Compte commission</label>
                  <input type="text" id="comm" name="comm" class="form-control" maxlength="11" pattern=".{11,}" title="Le compte doit avoir au moins 11 caractères" required>
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

  function modifier2(ref_igor, etat, date) {
    $("#ref_igor2").val(ref_igor);
    $("#etat2").val(etat);
    $("#date_regul2").val(date);
  }


</script>

<script>
function ouvrirEtImprimer(date_oper, ref_igor, montant, oper, compte) {

    var url = '<?php echo site_url('Imprimer/imprimerDocument'); ?>' +
      '?date=' + encodeURIComponent(date_oper) +
      '&ref=' + encodeURIComponent(ref_igor) +
      '&oper=' + encodeURIComponent(oper)+
      '&montant=' + encodeURIComponent(montant)+
      '&compte=' + encodeURIComponent(compte);

    var nouvelleFenetre = window.open(url, '_blank');

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


  $("#modifForm").submit(function(e) {
    e.preventDefault();
    const etat = $("#etat").val();
    const date_regul= $("#date_regul").val();
    const ref_igor = $("#ref_igor").val();



    $.ajax({
        url: "<?php echo site_url("airtel/edit_anomalie_igor_ci") ?>",
        type: "POST",
        data: {
            "ref_igor": ref_igor,
            "date_regul": date_regul,
            "etat": etat
        },
        success: function(response) {
            window.location.reload();
    
        },
        error: function(error) {
            console.log(error);
        }
    });    
  });

 

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
      place= "Directeur Déléguée à la Monétique (p.i)";
    }


    console.log(compte, dateOper, montant, libelle, oper, refIgor);

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
      "&operateur="  + encodeURIComponent("AIRTEL") +
      "&mobile_money=" + encodeURIComponent("AIRTEL MONEY") + 
      "&tax="  + encodeURIComponent("333,33") +
      "&tva=" + encodeURIComponent("66,67") +
      "&montant_comm=" + encodeURIComponent("400,00") +
      '&place=' +  encodeURIComponent(place)

    var nouvelleFenetre = window.open(url, '_blank'); 
      $('#myModalFr10').modal('hide');
      $("#compte").val("");
      $("#comm").val("");
      $("#nom").val("");
});


  $("#modifForm2").submit(function(e) {
    e.preventDefault();
    const etat = $("#etat2").val();
    const date_regul= $("#date_regul2").val();
    const ref_igor = $("#ref_igor2").val();

    $.ajax({
        url: "<?php echo site_url("airtel/edit_anomalie_igor_co") ?>",
        type: "POST",
        data: {
            "ref_igor": ref_igor,
            "date_regul": date_regul,
            "etat": etat
        },
        success: function(response) {
            // window.location.reload();
    
        },
        error: function(error) {
            console.log(error);
        }
    });    
  });



$("#tableNormaleCashOut").DataTable({
  responsive: true,
  scrollX: true,
  autoWidth: false,
  order: [],
  language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
  }
});

  $('#tableNormaleCashIn').DataTable( {
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });
  $("#tableDat").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

$("#tableCat").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

  $("#tableDeallo").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

  $("#tableMvtsCO").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });
  $("#tableMvtsCI").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });
  $("#tableAmbi").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });


  $("#tableSuccesCO").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });
  $("#tableSuccesCI").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });
  $("#tableAmbiCO").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }

  });

  $("#tableVI").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

    // $("#tableAnormaleCashOut").DataTable();
  $(".nav-tabs a").click(function(){
    $(this).tab('show');
    $($.fn.dataTable.tables(true)).DataTable()
      .columns.adjust();
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


</script>
