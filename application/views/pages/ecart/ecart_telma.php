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
      <a class="nav-link" data-toggle="tab" href="#telma">Telma</a>
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
                    <?php $montant_precedent= 0; ?>
                    <?php foreach($boa as $item) { ?>
                        <tr>
                            <td><?= $item->COMPTE ?></td>
                            <td><?= $item->DATE_OPER ?></td>
                            <td><?= $item->DATE_VAL ?></td>
                            <td><?= $item->DEVISE ?></td>
                            <td><?= number_format($item->MONTANT, 0, ',', ' ') ?></td>
                            <td><?= $item->LIBELLE ?></td>
                            <td><?= $item->OPER ?></td>
                            <td><?= $item->EXPL ?></td>
                            <td><?= $item->REF_IGOR ?></td>
                            <td><?= number_format($montant_precedent+= $item->MONTANT, 0, ',', ' ') ?></td>
                        </tr>
                    <?php } ?>
                    

                </tbody>
            </table>
        </div>
    </div>
    <div id="telma" class="container tab-pane fade"><br>
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
        <div class="table-responsive mt-2">
            <table class="table table-bordered" id="tableTelma">
                <thead>
                    <tr>
                        <th>Date</th>
                        <th>Transfer_id</th>
                        <th>Type</th>
                        <th>Channel</th>
                        <th>Etat</th>
                        <th>Montant</th>
                        <th>sender</th>
                        <th>receiver</th>
                        <th>Solde</th>
                    </tr>
                </thead>
                <tbody>
                    <?php $montant_precedent=0; ?>
                    <?php foreach($telma as $item) { ?>
                        <tr>
                            <td><?php echo $item->date ?></td>
                            <td><?= $item->trans_id ?></td>
                            <td><?= $item->TYPE ?></td>
                            <td><?= $item->channel ?></td>
                            <td><?= $item->state ?></td>
                            <td><?= number_format($item->Amount_MGA, 0, ',', ' ') ?></td>
                            <td><?= $item->sender ?></td>
                            <td><?= $item->receiver ?></td>
                            <td><?= number_format($montant_precedent+= $item->solde, 0, ',', ' ') ?></td>
                        </tr>
                    <?php }?>
                </tbody>
            </table>
        </div>
    </div>
  </div>
</div>

<script>
$("#tableTelma").DataTable({
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

