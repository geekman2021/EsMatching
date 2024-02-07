<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>

<div class="container-fluid mt-5">
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

<div id="tabNormaleCashOut" class="container-fluid tab-pane "><br>
    <div class="container-fluid table-responsive" >
        <table class="table table-bordered" id="tableNormaleCashIn">
            <thead style="text-align: center ;">
                <tr>
                    <th colspan="11" style="text-align: center ;">PRINCIPAL</th>
                    <th></th>
                    <th colspan="12" style="text-align: center ;">COMMISSION</th>
                    <th colspan="15" style="text-align: center ;">ORANGE</th>
                </tr>
                <tr>
                    <th>DATE_OPER</th>
                    <th>DATE_VAL</th>
                    <th>DEVISE</th>
                    <th>MONTANT</th>
                    <th>LIBELLE</th>
                    <th>CODE_OPER</th>
                    <th>EXPL</th>
                    <th>REF_IGOR</th>
                    <th>REF_REL</th>
                    <th>PHONE</th>
                    <th>SOLDE</th>
                    <th style="background: white;"></th>
                    <th>DATE_OPER</th>
                    <th>DATE_VAL</th>
                    <th>DEVISE</th>
                    <th>MONTANT</th>
                    <th>LIBELLE</th>
                    <th>CODE_OPER</th>
                    <th>EXPL</th>
                    <th>REF_IGOR</th>
                    <th>OPER_ID</th>
                    <th>CODE_AGENCE</th>
                    <th>SOLDE</th>
                    <th></th>


                    <th>DATE</th>
                    <th>HEURE</th>
                    <th>REFERENCE</th>
                    <th>SERVICE</th>
                    <th>NUM_COMPTE</th>
                    <th>DEBIT</th>
                    <th>CREDIT</th>
                    <th>MONTANT</th>
                    <th>SOLDE</th>
                    <th>DEBIT</th>
                    <th>CREDIT</th>
                    <th>MONTANT</th>
                    <th>SOLDE</th>
                    <th>PRINCIPAL</th>
                    <th>COMM</th>
                    

                </tr>
            </thead>
            <tbody>
                <?php $princ_solde= 0; $comm_solde= 0; $orange_solde= 0;?>
                
                <?php foreach($historique as $item) { ?>
                <tr>
                <td><?= $item->princ_date_oper ?></td>
                <td><?= $item->princ_date_val ?></td>
                <td><?= $item->princ_devise ?></td>
                <td><?= $item->princ_montant ?></td>
                <td><?= $item->princ_libelle ?></td>
                <td><?= $item->princ_oper ?></td>
                <td><?= $item->princ_expl ?></td>
                <td><?= $item->princ_ref_igor ?></td>
                <td><?= $item->princ_ref_rel ?></td>
                <td><?= substr($item->cle, 0, 10)?></td>
                <td><?= number_format($princ_solde += $item->princ_montant, 0, ',', ' '); ?></td>

                <td style="background: white;"></td>

                <td><?= $item->comm_date_oper ?></td>
                <td><?= $item->comm_date_val ?></td>
                <td><?= $item->comm_devise ?></td>
                <td><?= $item->comm_montant ?></td>
                <td><?= $item->comm_libelle ?></td>
                <td><?= $item->comm_oper ?></td>
                <td><?= $item->comm_expl ?></td>
                <td><?= $item->comm_ref_igor ?></td>
                <td><?= $item->comm_ref_rel ?></td>
                <td><?= $item->comm_code_agence ?></td>
                <td><?= number_format($comm_solde += $item->comm_montant, 0, ',', ' ') ?></td>

                <td style="background: white;"></td>
                
                <td><?= $item->orange_date ?></td>
                <td><?= $item->orange_heure ?></td>
                <td><?= $item->orange_ref ?></td>
                <td><?= $item->orange_service ?></td>
                <td><?= $item->orange_num_compte ?></td>
                <td><?= $item->orange_debit ?></td>
                <td><?= $item->orange_credit ?></td>
                <td><?= $item->orange_montant ?></td>
                <td><?= number_format($orange_solde += $item->orange_montant, 0, ',', ' ') ?></td>
                <td><?= $item->orange_sous_distri ?></td>
                <td><?= $item->orange_super_distri ?></td>
                <td><?= $item->orange_super_distri ?></td>
                <td><?= number_format($orange_solde += $item->orange_montant, 0, ',', ' '); ?></td>
                <td><?= $item->princ_montant + $item->orange_montant  ?></td>
                <td><?= $item->comm_montant + $item->orange_super_distri ?></td>
                </tr>

                <?php  } ;?>
            </tbody>
        </table>
    </div>
</div>


    <!-- <script>
        $(document).ready(function () {
            $("#tableNormaleCashIn").DataTable( {
                scrollX: true,
                dom: 'Bfrtip',
                buttons: [
                    'copy', 'csv', 'excel', 'pdf', 'print'
                    ],
                language: {
                url: '<?php echo base_url(); ?>assets/fr-FR.json',
                ordering: false
            }
    });
        })
    </script> -->

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
    let table = new DataTable('#tableNormaleCashIn', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
    });

    table.order([]).draw();
    document.querySelectorAll('#min, #max').forEach((el) => {
        el.addEventListener('change', () => table.draw());
    });
 
</script>

<script>
    document.getElementById('exportExcel').addEventListener('click', function() {

    $.ajax({
        url: '<?php echo site_url("Historique_Orange/exporterVersExcel") ?>',
        success: function(response) {
            console.log(response);
            window.location.href = '<?php echo site_url("Historique_Orange/exporterVersExcel") ?>';
        },
        error: function(xhr, status, error) {
        console.error(error);
        }
    });
    });

</script>