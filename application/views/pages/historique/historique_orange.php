<div class="container mt-5">
    <div class="row">
        <div class="form-group col-sm-6" >
            <label for="date_debut">Date Debut</label>
            <input type="date" class="form-control" id="dateDebut">
        </div>
        <div class="form-group col-sm-6" >
            <label for="date_fin">Date Fin</label>
            <input type="date" class="form-control" id="dateFin">
        </div>
    </div>
</div>

<div id="tabNormaleCashOut" style="padding-left: 5vw; padding-right: 5vw;" class="tab-pane "><br>
    <div class="table-responsive" ">
        <table class="table table-bordered" id="tableNormaleCashIn">
            <thead style="text-align: center ;">
                <tr>
                    <th colspan="12" style="text-align: center ;">BOA</th>
                    <th style="text-align: center ;"></th>
                    <th colspan="9" style="text-align: center ;">ORANGE</th>
                </tr>
                <tr>
                    <th></th>
                </tr>
                <tr>
                    <th>Code_Agence</th>
                    <th>Date_Oper</th>
                    <th>princ_date_val</th>
                    <th>comm_date_val</th>
                    <th>princ_montant</th>
                    <th>comm_montant</th>
                    <th>Montant</th>
                    <th>Devise</th>
                    <th>Oper</th>
                    <th>Expl</th>
                    <th>ReferenceIgor</th>
                    <th>Cle</th>
                    <th></th>
                    <th>Cle</th>
                    <th></th>
                    <th>Date</th>
                    <th>Heure</th>
                    <th>Réference</th>
                    <th>Service</th>
                    <th>Num_Compte</th>
                    <th>Débit</th>
                    <th>Crédit</th>
                </tr>
            </thead>
            <tbody>
                
                <?php foreach($historique as $item) { ?>
                <tr>
                <td><?php echo $item->comm_code_agence ?></td>
                <td><?php echo $item->princ_date_oper ?></td>
                <td><?php echo $item->princ_date_val ?></td>
                <td><?php echo $item->comm_date_val ?></td>
                <td><?php echo $item->princ_montant ?></td>
                <td><?php echo $item->comm_montant ?></td>
                <td><?php echo "" ?></td>
                <td><?php echo $item->princ_devise?></td>
                <td><?php echo $item->princ_oper ?></td>
                <td><?php echo $item->princ_expl ?></td>
                <td><?php echo $item->princ_ref_igor ?></td>
                <td><?php echo $item->cle ?></td>
                <td><?php echo $item->orange_date ?></td>
                <td><?php echo $item->orange_heure ?></td>
                <td><?php echo $item->orange_ref ?></td>
                <td><?php echo $item->orange_service ?></td>
                <td><?php echo $item->orange_num_compte ?></td>
                <td><?php echo $item->orange_debit ?></td>
                <td><?php echo $item->orange_credit ?></td>
                <td><?php echo $item->orange_credit ?></td>
                <td><?php echo $item->orange_credit ?></td>
                <td><?php echo $item->orange_credit ?></td>
                </tr>

                <?php  } ;?>
            </tbody>
        </table>
    </div>
</div>


    <script>
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
    </script>