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
      <a class="nav-link" data-toggle="tab" href="#orange">Orange</a>
    </li>
  </ul>

<div class="tab-content">
    <div id="boa" class="container tab-pane fade"><br>
        <!-- <div class="container">
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
        </div> -->
        <div class="table-responsive mt-5">
        <table class="table table-bordered" id="tableBoa">
                <thead>
                    <tr>
                      <th>Code_Agence</th>
                      <th>Date_Oper</th>
                      <th>princ_date_val</th>
                      <th>comm_date_val</th>
                      <th>princ_montant</th>
                      <th>comm_montant</th>
                      <th>Devise</th>
                      <th>Oper</th>
                      <th>Expl</th>
                      <th>ReferenceIgor</th>
                      <th>Reference_rel</th>
                      <th>Numero</th>
                    </tr>
                </thead>
                <tbody>
                    <?php foreach ($ecart_boa as $item) { ?>
                        <tr>
                            <td><?= $item->comm_code_agence ?></td>
                            <td><?= $item->princ_date_oper ?></td>
                            <td><?= $item->princ_date_val ?></td>
                            <td><?= $item->comm_date_val ?></td>
                            <td><?= $item->princ_montant ?></td>
                            <td><?= $item->comm_montant ?></td>
                            <td><?= $item->princ_devise ?></td>
                            <td><?= $item->princ_oper ?></td>
                            <td><?= $item->princ_expl ?></td>
                            <td><?= $item->princ_ref_igor ?></td>
                            <td><?= $item->princ_ref_rel ?></td>
                            <td><?= substr($item->cle, 0, 3) . " " . substr($item->cle, 3, 2) . " " . substr($item->cle, 5, 3) . " " . " " . substr($item->cle, 8, 2) ?></td>
                        </tr>
                    <?php } ?>

                </tbody>
            </table>
        </div>
    </div>
    <div id="orange" class="container tab-pane fade"><br>
        <!-- <div class="container">
            <div class="row"> -->
                <!-- <div class="col-sm-6">
                    <label for="dateDebut">Date Debut</label>
                    <input type="text" class="form-control" id="min" name="min">
                </div>
                <div class="col-sm-6">
                    <label for="dateDebut">Date Fin</label>
                    <input type="text" class="form-control" id="max" name="max">
                </div>  -->
                <!-- <div class="col-sm-12 mt-2">
                    <button id="exportExcel" class="btn btn-success">Exporter vers Excel </button>
                </div> -->
            <!-- </div>
        </div> -->
        <div class="table-responsive">
            <table class="table table-bordered" id="tableOrange">
                <thead>
                    <tr>
                        <th>Date</th>
                        <th>Heure</th>
                        <th>Réference</th>
                        <th>Service</th>
                        <th>Num_compte</th>
                        <th>Débit</th>
                        <th>Crédit</th>
                        <th>Numero</th>
                    </tr>
                </thead>
                <tbody>
                    <?php foreach($ecart_orange as $item) { ?>
                        <tr>
                            <td><?= $item->orange_date ?></td>
                            <td><?= $item->orange_heure ?></td>
                            <td><?= $item->orange_ref ?></td>
                            <td><?= $item->orange_service ?></td>
                            <td><?= $item->orange_num_compte ?></td>
                            <td><?= $item->orange_debit ?></td>
                            <td><?= $item->orange_credit ?></td>
                            <td><?= substr($item->cle, 0, 3) . " " . substr($item->cle, 3, 2) . " " . substr($item->cle, 5, 3) . " " . " " . substr($item->cle, 8, 2) ?></td>
                        </tr>
                    <?php } ?>

                </tbody>
            </table>
        </div>
    </div>
  </div>
</div>

<script>

$("#tableOrange").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});

$("#tableBoa").DataTable({
    responsive: true,
    scrollX: true,
    autoWidth: false,
    order: [],
    language: {
      url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
});

$(".nav-tabs a").click(function(){
  $(this).tab('show');
  $($.fn.dataTable.tables(true)).DataTable()
      .columns.adjust();
});

</script>

