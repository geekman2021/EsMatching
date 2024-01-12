<?php $this->load->view("pages/ecart/ecart") ?>

<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>


<div class="container mt-5">
  <ul class="nav nav-tabs" role="tablist">
    <li class="nav-item col-6">
      <a class="nav-link" data-toggle="tab" href="#boa">BOA</a>
    </li>
    <li class="nav-item col-6">
      <a class="nav-link" data-toggle="tab" href="#airtel">Airtel</a>
    </li>
  </ul>

<div class="tab-content">
    <div id="boa" class="container tab-pane fade"><br>
        <div class="container">
            <div class="row">
                <div class="col-sm-6">
                    <label for="dateDebut">Date Debut</label>
                    <input type="text" class="form-control" id="min" name="min">
                </div>
                <div class="col-sm-6">
                    <label for="dateDebut">Date Fin</label>
                    <input type="text" class="form-control" id="max" name="max">
                </div> 
                <div class="col-sm-12 mt-2">
                    <button id="exportExcel" class="btn btn-success">Exporter vers Excel </button>
                </div>
            </div>
        </div>
        <div class="table-responsive mt-5">
        <table class="table table-bordered" id="tableBoa">
                <thead>
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
                        <th>Solde</th>
                    </tr>
                </thead>
                <tbody>
                    <?php $montant_precedent= 0;  ?>
                    <?php foreach($ecart_boa as $item) { ?>
                        <tr>
                            <td><?= $item->COMPTE ?></td>
                            <td><?= $item->DATE_OPER ?></td>
                            <td><?= $item->DATE_VAL  ?></td>
                            <td><?= $item->DEVISE ?></td>
                            <td style="white-space: nowrap;"><?= number_format($item->MONTANT, 0, ',', ' ') ?></td>
                            <td><?= $item->LIBELLE ?></td>
                            <td><?= $item->OPER ?></td>
                            <td><?= $item->EXPL ?></td>
                            <td><?= $item->REF_IGOR ?></td>
                            <td style="white-space: nowrap;"><?= number_format($montant_precedent += $item->MONTANT, 0, ',', ' ')  ?> </td>
                        </tr>
                    <?php } ?>
                </tbody>
            </table>
        </div>
    </div>
    <div id="airtel" class="container tab-pane fade"><br>
        <div class="container">
            <div class="row">
                <div class="col-sm-6">
                    <label for="dateDebut">Date Debut</label>
                    <input type="text" class="form-control" id="min" name="min">
                </div>
                <div class="col-sm-6">
                    <label for="dateDebut">Date Fin</label>
                    <input type="text" class="form-control" id="max" name="max">
                </div> 
                <div class="col-sm-12 mt-2">
                    <button id="exportExcel" class="btn btn-success">Exporter vers Excel </button>
                </div>
            </div>
        </div>
        <div class="table-responsive">
            <table class="table table-bordered" id="tableAirtel">
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
                        <th>Solde</th>
                    </tr>
                </thead>
                <tbody>
                    <?php $montant_precedent = 0; ?>
                    <?php foreach($ecart_airtel as $item) { ?>
                        <tr>
                            <td><?= $item->TRANSFER_ID ?></td>
                            <td><?= $item->transfer_date ?></td>
                            <td><?= $item->external_id ?></td>
                            <td><?= $item->account_no ?></td>
                            <td><?= $item->sender_msisdn ?></td>
                            <td><?= $item->dest_msisdn ?></td>
                            <td style="white-space: nowrap;"><?= number_format($item->amount, 0, ',', ' ' )  ?></td>
                            <td><?= $item->description ?></td>
                            <td><?= $item->service_name ?></td>
                            <td><?= $item->reference_number ?></td>
                            <td style="white-space: nowrap;"><?= number_format($montant_precedent += $item->solde, 0, ',', ' ')?></td>
                        </tr>
                    <?php } ?>
                </tbody>
            </table>
        </div>
    </div>
  </div>
</div>

<script>
$("#tableAirtel").DataTable({
    responsive: true,
    scrollX: true,
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});

$("#tableBoa").DataTable({
    responsive: true,
    scrollX: true,
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});

</script>

