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
                    </tr>
                </thead>
                <tbody>

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
                    </tr>
                </thead>
                <tbody>

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

