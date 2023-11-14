<div id="tabAmbigu" class="container-fluid mt-5 tab-pane ">
    <div class="row">
      <div class="col-sm-6 pl-4">
          <label for="dateDebut">Date Debut</label>
          <input type="text" class="form-control" id="minAirtel" name="minAirtel">
      </div>
        <div class="col-sm-6 pr-4">
          <label for="dateDebut">Date Fin</label>
          <input type="text" class="form-control" id="maxAirtel" name="maxAirtel">
        </div> 
    </div>
    <div class="container-fluid mt-3">
      <div class="card">
        <div class="card-header">
          <b>Liste Ambiguous Regularisé</b>
        </div>
        <div class="card-body">
        <table class="table table-bordered" id="tableAmbigu">
          <thead>
            <tr>
              <th>External_id</th>
              <th>id_transfer </th>
              <th>Transfer_date</th>
              <th>Compte</th>
              <th>sender_msiSDN</th>
              <th>Dest_MsiSDN</th>
              <th>Montant</th>
              <th>Description</th>
              <th>Nom_Service</th>
              <th>Num_réference</th>
              <th>Action</th>
            </tr>
          </thead>
          <tbody>
          <?php if (!empty($ambi)) { ?>
            <?php foreach($ambi as $item)  { ?>
              <tr> 
                <td><?php echo $item->external_id; ?></td>
                <td><?php echo $item->TRANSFER_ID; ?></td>
                <td><?php echo $item->transfer_date; ?></td>
                <td><?php echo $item->account_no; ?></td>
                <td><?php echo $item->sender_msisdn; ?></td>
                <td><?php echo $item->dest_msisdn; ?></td>
                <td><?php echo number_format($item->amount, 0, ',', ' '); ?></td>
                <td><?php echo $item->description; ?></td>
                <td><?php echo $item->service_name; ?></td>
                <td><?php echo $item->reference_number; ?></td>
                <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalAmbigu<?= $item->id_ambiguous ?>" class="btn btn-info"><i class="bx bx-show" style="font-size: 24px;"></i></a></td>
              </tr>
            <?php } ?>
          <?php } ?>
        </tbody>
      </table>
        </div>
      </div>
    </div>
    <div class="row">
      <div class="col-sm-6 pl-4">
          <label for="dateDebut">Date Debut</label>
          <input type="text" class="form-control" id="minAirtel" name="minAirtel">
      </div>
        <div class="col-sm-6 pr-4">
          <label for="dateDebut">Date Fin</label>
          <input type="text" class="form-control" id="maxAirtel" name="maxAirtel">
        </div> 
    </div>
    <div class="container-fluid mt-3">
      <div class="card">
        <div class="card-header">
          <b>Liste Ambiguous Regularisé</b>
        </div>
        <div class="card-body">
        <table class="table table-bordered" id="tableAmbiguCO">
          <thead>
            <tr>
              <th>External_id</th>
              <th>id_transfer </th>
              <th>Transfer_date</th>
              <th>Compte</th>
              <th>sender_msiSDN</th>
              <th>Dest_MsiSDN</th>
              <th>Montant</th>
              <th>Description</th>
              <th>Nom_Service</th>
              <th>Num_réference</th>
              <th>Action</th>
            </tr>
          </thead>
          <tbody>
          
          <?php if (!empty($ambi_co)) { ?>
            <?php foreach($ambi_co as $item)  { ?>
              <tr> 
                <td><?php echo $item->external_id; ?></td>
                <td><?php echo $item->TRANSFER_ID; ?></td>
                <td><?php echo $item->transfer_date; ?></td>
                <td><?php echo $item->account_no; ?></td>
                <td><?php echo $item->sender_msisdn; ?></td>
                <td><?php echo $item->dest_msisdn; ?></td>
                <td><?php echo number_format($item->amount, 0, ',', ' '); ?></td>
                <td><?php echo $item->description; ?></td>
                <td><?php echo $item->service_name; ?></td>
                <td><?php echo $item->reference_number; ?></td>
                <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalAmbiguCO<?= $item->id_ambiguous ?>" class="btn btn-info"><i class="bx bx-show" style="font-size: 24px;"></i></a></td>
              </tr>
            <?php } ?>
          <?php } ?>
        </tbody>
      </table>
        </div>
      </div>
    </div>
  </div>

  <!-- <div class="row">
      <div class="col-sm-6 pl-4">
          <label for="dateDebut">Date Debut</label>
          <input type="text" class="form-control" id="minAirtel" name="minAirtel">
      </div>
        <div class="col-sm-6 pr-4">
          <label for="dateDebut">Date Fin</label>
          <input type="text" class="form-control" id="maxAirtel" name="maxAirtel">
        </div> 
    </div>
    <div class="container-fluid mt-3">
      <div class="card">
        <div class="card-header">
          <b>Liste Ambiguous Regularisé</b>
        </div>
        <div class="card-body">
        <table class="table table-bordered" id="tableAmbiguCO">
          <thead>
            <tr>
              <th>External_id</th>
              <th>id_transfer </th>
              <th>Transfer_date</th>
              <th>Compte</th>
              <th>sender_msiSDN</th>
              <th>Dest_MsiSDN</th>
              <th>Montant</th>
              <th>Description</th>
              <th>Nom_Service</th>
              <th>Num_réference</th>
              <th>Action</th>
            </tr>
          </thead>
          <tbody>
          <?php if (!empty($ambi)) { ?>
            <?php foreach($ambi as $item)  { ?>
              <tr> 
                <td><?php echo $item->external_id; ?></td>
                <td><?php echo $item->TRANSFER_ID; ?></td>
                <td><?php echo $item->transfer_date; ?></td>
                <td><?php echo $item->account_no; ?></td>
                <td><?php echo $item->sender_msisdn; ?></td>
                <td><?php echo $item->dest_msisdn; ?></td>
                <td><?php echo number_format($item->amount, 0, ',', ' '); ?></td>
                <td><?php echo $item->description; ?></td>
                <td><?php echo $item->service_name; ?></td>
                <td><?php echo $item->reference_number; ?></td>
                <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalAmbigu<?= $item->id_ambiguous ?>" class="btn btn-info"><i class="bx bx-show" style="font-size: 24px;"></i></a></td>
              </tr>
            <?php } ?>
          <?php } ?>
        </tbody>
      </table>
        </div>
      </div>
    </div>
  </div> -->


  <?php $this->load->view("pages/regul/airtel/modal/modalAmbiguous") ?>