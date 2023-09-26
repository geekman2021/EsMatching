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
        <table class="table table-bordered" id="tableNormaleCashOut">
        <thead style="text-align: center ;">
            <tr>
                <th colspan="10" style="text-align: center ;">BOA</th>
                <th colspan="11" style="text-align: center ;">Airtel</th>
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
                <th>Transfer_Id</th>
                <th>Transfer_Date</th>
                <th>Reference_Igor</th>
                <th>Solde</th>
                <th style="background-color: green;"></th>
                <th>External_Id</th>
                <th>Account_No</th>
                <th>Sender_MsiSDN</th>
                <th>Dest_MsiSDN</th>
                <th>Amount</th>
                <th>Description</th>
                <th>Service_Name</th>
                <th>Reference_Number</th>
                <th>Solde</th>
            </tr>
        </thead>
            <tbody>
                <?php foreach($historique_airtel as $item) { ?>
                <tr>
                    <td style="display: none;"><?php echo $item->id; ?></td>
                    <td><?php echo $item->date_oper ?></td>
                    <td><?php echo $item->date_val ?></td>
                    <td><?php echo $item->compte ?></td>
                    <td><?php echo $item->montant ?></td>
                    <td><?php echo $item->devise ?></td>
                    <td><?php echo $item->oper ?></td>
                    <td><?php echo $item->expl ?></td>
                    <td><?php echo $item->transfer_id ?></td>
                    <td><?php echo $item->transfer_date ?></td>
                    <td ><?php echo $item->ref_igor ?></td>
                    <td><?php echo $item->solde_boa ?></td>
                    <td></td>
                    <td><?php echo $item->external_id ?></td>
                    <td><?php echo $item->account_no ?></td>
                    <td><?php echo $item->sender_msisdn ?></td>
                    <td><?php echo $item->dest_msisdn ?></td>
                    <td><?php echo $item->amount ?></td>
                    <td style="white-space: nowrap; overflow: hidden; text-overflow: ellipsis ;max-width: 100px; "><?php echo $item->description ?></td>
                    <td style="white-space: nowrap; overflow: hidden; text-overflow: ellipsis ;max-width: 100px; "><?php echo $item->service_name ?></td>
                    <td style="white-space: nowrap; overflow: hidden; text-overflow: ellipsis ;max-width: 100px; "><?php echo $item->reference_number ?></td>
                    <td><?php echo $item->solde_airtel ?></td>

                </tr>
                <?php } ?>
            
            </tbody>
        </table>
    </div>
</div>


    <script>
        $(document).ready(function () {
            $("#tableNormaleCashOut").DataTable( {
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