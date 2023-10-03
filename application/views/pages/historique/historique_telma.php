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
                <th colspan="9" style="text-align: center ;">BOA</th>
                <th colspan="1"></th>
                <th colspan="14" style="text-align: center ;">Telma</th>
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
                <th>cle</th>

                <th>Date</th>
                <th>Heure</th>
                <th>Transfer_id</th>
                <th>Type</th>
                <th>Channel</th>
                <th>Etat</th>
                <th>Montant</th>
                <th>Action</th>
                <th>sender</th>
                <th>receiver</th>
                <th>Solde</th>
                
            </tr>
        </thead>
            <tbody>
                <?php foreach($historique as $item) { ?>
                <tr>
                    <td style="display: none;"><?php echo $item->id; ?></td>
                    <td><?php echo $item->DATE_OPER ?></td>
                    <td><?php echo $item->DATE_VAL ?></td>
                    <td><?php echo $item->COMPTE ?></td>
                    <td><?php echo $item->MONTANT ?></td>
                    <td><?php echo $item->DEVISE ?></td>
                    <td><?php echo $item->OPER ?></td>
                    <td><?php echo $item->EXPL ?></td>
                    <td ><?php echo $item->REF_IGOR ?></td>
                    <td><?php echo $item->solde_boa ?></td>
                    <td><?php echo $item->cle ?></td>
                      <td><?php echo substr($item->date_d, 0, 10) ?></td>
                      <td><?php echo substr($item->date_d, 10) ?></td>
                      <td><?php echo $item->trans_id ?></td>
                      <td><?php echo $item->TYPE ?></td>
                      <td><?php echo $item->channel ?></td>
                      <td><?php echo $item->state ?></td>
                      <td><?php echo $item->Amount_MGA ?></td>
                      <td><?php echo $item->ACTION ?></td>
                      <td><?php echo $item->sender ?></td>
                      <td><?php echo $item->receiver ?></td>
                      <td><?php echo $item->solde_telma ?></td>
                </tr>
                <?php } ?>
            
            </tbody>
        </table>
    </div>
</div>


    <script>
        $(document).ready(function () {
            $("#tableNormaleCashOut").DataTable( {
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