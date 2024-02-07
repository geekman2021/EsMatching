<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>
<div class="container mt-5">
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

<div id="tabNormaleCashOut" style="padding-left: 5vw; padding-right: 5vw;" class="tab-pane "><br>
    <div class="table-responsive" ">
        <table class="table table-bordered" id="tableNormaleCashOut">
        <thead style="text-align: center ;">
            <tr>
                <th colspan="9" style="text-align: center ;">BOA</th>
                <th style="background-color: rgba(242, 242, 242, 0.5);"></th>
                <th colspan="12" style="text-align: center ;">Airtel</th>
                <th style="text-align: center ;">Principal</th>
            </tr>
            <tr>
                <th style="display: none;"></th>
                <th>Date_Oper</th>
                <th>Date_Val</th>
                <th>Compte</th>
                <th>Montant</th>
                <th>Devise</th>
                <th>Operation</th>
                <th>Expl</th>
                <th>Reference_Igor</th>
                <th>Solde</th>
                <th style="background-color: rgba(242, 242, 242, 0.5);"></th>
                <th>Transfer_Id</th>
                <th>Transfer_Date</th>
                <th>External_Id</th>
                <th>Account_No</th>
                <th>Sender_MsiSDN</th>
                <th>Dest_MsiSDN</th>
                <th>Amount</th>
                <th>Description</th>
                <th>Service_Name</th>
                <th>Reference_Number</th>
                <th>Montant</th>
                <th>Solde</th>
                <th>Ecart</th>
            </tr>
        </thead>
            <tbody>
                <?php $solde_precedent= 0; ?>
                <?php foreach($historique as $item) { ?>
                <tr>
                    <td style="display: none;"><?php echo $item->id; ?></td>
                    <td><?php echo $item->DATE_OPER ?></td>
                    <td><?php echo $item->DATE_VAL ?></td>
                    <td><?php echo $item->COMPTE ?></td>
                    <td><?php echo number_format($item->MONTANT, 0, ',', ' ') ?></td>
                    <td><?php echo $item->DEVISE ?></td>
                    <td><?php echo $item->OPER ?></td>
                    <td><?php echo $item->EXPL ?></td>

                    <td ><?php echo $item->REF_IGOR ?></td>
                    <td><?php echo number_format($item->solde_boa, 0, ',', ' ') ?></td>
                    <td style="background-color: rgba(242, 242, 242, 0.5);"></td>
                    <td><?php echo $item->TRANSFER_ID ?></td>
                    <td><?php echo $item->transfer_date ?></td>
                    <td><?php echo $item->external_id ?></td>
                    <td><?php echo $item->account_no ?></td>
                    <td><?php echo $item->sender_msisdn ?></td>
                    <td><?php echo $item->dest_msisdn ?></td>
                    <td><?php echo number_format($item->amount, 0, ',', ' ') ?></td>
                    <td><?php echo $item->description ?></td>
                    <td><?php echo $item->service_name ?></td>
                    <td><?php echo $item->reference_number ?></td>
                    <td><?php echo number_format($item->solde, 0, ',', ' ') ?></td>
                    <td><?php echo number_format(($solde_precedent += $item->solde), 0, ',', ' ') ?></td>
                    <td><?php echo $item->solde_airtel + $item->solde_boa  ?></td>
                </tr>
                <?php } ?>
            
            </tbody>
        </table>
    </div>
</div>


<script>

    let minDate, maxDate;
    DataTable.ext.search.push(function (settings, data, dataIndex) {
        let min = minDate.val();
        let max = maxDate.val();
        let date = new Date(data[1]);
    
        if (
            (min === null && max === null) ||
            (min === null && date <= max) ||
            (min <= date && max === null) ||
            (min <= date && date <= max)
        ) {
            return true;
        }
        return false;
    });
    
    // Create date inputs
    minDate = new DateTime('#min', {
        format: 'LL', 
        locale: 'fr', 
    });

    maxDate = new DateTime('#max', {
        format: 'LL', 
        locale: 'fr', 
    });
    
    // DataTables initialisation
    let table = new DataTable('#tableNormaleCashOut', {
                scrollX: true,
                language: {
                    url: '<?php echo base_url(); ?>assets/fr-FR.json',
                }
            });

    document.querySelectorAll('#min, #max').forEach((el) => {
        el.addEventListener('change', () => table.draw());
    });
 
</script>

<script>
    document.getElementById('exportExcel').addEventListener('click', function() {

    $.ajax({
        url: '<?php echo site_url("Historique_Airtel/exporterVersExcel") ?>',
        success: function(response) {
            console.log(response);
            window.location.href = '<?php echo site_url("Historique_Airtel/exporterVersExcel") ?>';
        },
        error: function(xhr, status, error) {
        console.error(error);
        }
    });
    });

</script>



