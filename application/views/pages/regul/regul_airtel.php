<div class="mt-5 container"><br>
    <ul class="nav nav-tabs">
      <li class="nav-item col-sm-6">
          <a class="nav-link" data-toggle="tab" href="#tabBoa">BOA</a>
      </li>
      <li class="nav-item col-sm-6">
          <a class="nav-link" data-toggle="tab" href="#tabAirtel">Airtel</a>
      </li>
    </ul>
</div>

<!-- ONGLET BOA -->
<div class="tab-content">

  <div id="tabBoa" class="container mt-1 tab-pane "><br>
    <div class="row">
        <div class="col-sm-6">
            <label for="dateDebut">Date Debut</label>
            <input type="text" class="form-control" id="minBoa" name="minBoa">
        </div>
        <div class="col-sm-6">
          <label for="dateDebut">Date Fin</label>
            <input type="text" class="form-control" id="maxBoa" name="maxBoa">
        </div> 
    </div>
    <div class="container mt-1">
    <table class="table table-bordered" id="tableBoa">
        <thead style="text-align: center ;">
            <tr>
              <th>Compte</th>
              <th>Date_Oper</th>
              <th>Date_Val </th>
              <th>Devise</th>
              <th>Montant</th>
              <th>Libelle</th>
              <th>Operation</th>
              <th>Expl</th>
              <th>ReferenceIgor</th>
              <th>Action</th>
            </tr>
        </thead>
        <tbody>
        <?php if (!empty($ci)) { ?>
          <?php foreach($ci as $item)  { ?>
            <tr> 
              <td><?php echo $item->COMPTE ?></td>
              <td><?php echo $item->DATE_OPER ?></td>
              <td><?php echo $item->DATE_VAL ?></td>
              <td><?php echo $item->DEVISE ?></td>
              <td><?php echo $item->MONTANT ?></td>
              <td><?php echo $item->LIBELLE ?></td>
              <td><?php echo $item->OPER ?></td>
              <td><?php echo $item->EXPL ?></td>
              <td><?php echo $item->REF_IGOR ?></td>
              <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalBoa<?php echo $item->id_nonau; ?>"><i class="bx bx-show" style="font-size: 20px"></i></a></td>
            </tr>
          <?php } ?>
        <?php } ?>
        <!-- <?php //if (!empty($deallo_vi)) { ?>
          <?php //foreach($deallo_vi as $item)  { ?>
            <tr> 
              <td><?= $item->COMPTE ?></td>
              <td><?= $item->DATE_OPER ?></td>
              <td><?= $item->DATE_VAL ?></td>
              <td><?= $item->DEVISE ?></td>
              <td><?= $item->MONTANT ?></td>
              <td><?= $item->LIBELLE ?></td>
              <td><?= $item->OPER ?></td>
              <td><?= $item->EXPL ?></td>
              <td><?= $item->REF_IGOR ?></td>
              <td><a href="#" data-toggle="modal" data-target="#myModalDeallo<?= $item->id; ?>">Afficher Infos</a></td>
            </tr>
          <?php //} ?>
        <?php //} ?> -->

        <?php if (!empty($deallo_vi)) : ?>
    <?php $afficheLigne = array(); ?>
    <?php foreach ($deallo_vi as $item) : ?>
        <?php
        $ligne = $item->COMPTE . $item->DATE_OPER . $item->DATE_VAL . $item->DEVISE . $item->MONTANT . $item->LIBELLE . $item->OPER . $item->EXPL . $item->REF_IGOR;
        if (!in_array($ligne, $afficheLigne)) :
            $afficheLigne[] = $ligne;
        ?>
            <tr>
                <td><?= $item->COMPTE ?></td>
                <td><?= $item->DATE_OPER ?></td>
                <td><?= $item->DATE_VAL ?></td>
                <td><?= $item->DEVISE ?></td>
                <td><?= $item->MONTANT ?></td>
                <td><?= $item->LIBELLE ?></td>
                <td><?= $item->OPER ?></td>
                <td><?= $item->EXPL ?></td>
                <td><?= $item->REF_IGOR ?></td>
                <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalDeallo<?= $item->REF_IGOR ?>"><i class="bx bx-show" style="font-size: 20px;"></i></a></td>
            </tr>
        <?php endif; ?>
    <?php endforeach; ?>
<?php endif; ?>

        </tbody>
      </table>
    </div>
  </div>
<!-- ONGLET AIRTEL -->

  <div id="tabAirtel" class="container mt-5 tab-pane ">
    <div class="row">
      <div class="col-sm-6">
          <label for="dateDebut">Date Debut</label>
          <input type="text" class="form-control" id="minAirtel" name="minAirtel">
      </div>
        <div class="col-sm-6">
          <label for="dateDebut">Date Fin</label>
          <input type="text" class="form-control" id="maxAirtel" name="maxAirtel">
        </div> 
    </div>
    <div class="container mt-3">
    <table class="table table-bordered" id="tableAirtel">
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
              <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalAirtel<?php echo $item->id_rollback; ?>" class="btn btn-info"><i class="bx bx-show" style="font-size: 24px;"></i></a></td>
            </tr>
          <?php } ?>
        <?php } ?>
      </tbody>
    </table>
    </div>
  </div>
</div>
</div>
</div>

  

<!-- MODAL AIRTEL  -->
<?php if (!empty($ambi)) { ?>
  <?php foreach($ambi as $item)  { ?>
    <div class="modal fade" id="myModalAirtel<?php echo $item->id_rollback; ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <div class="modal-header">
                    <h6 class="text-center">Infos</h6>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                  <div class="row">
                    <div class="col-sm-6">
                      <b>Régularisé</b>:
                      <?php
                        $dateObj = new DateTime($item->rollback_transfer_date);
                        $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                        $formattedDate2 = $formatter2->format($dateObj);
                        echo "le " .$formattedDate2 ?>
                    </div>
                    <div class="col-sm-6">
                    <b>Réf: </b><?php echo $item->rollback_reference_number; ?> </p>
                    </div>
                    <div class="col-sm-6">
                      <b>Service: </b><?php echo $item->rollback_service_name; ?> </p>
                    </div>
                    <div class="col-sm-6">
                      <b>Montant</b>: <?php echo $item->rollback_amount; ?> </p>
                    </div>
                    <div class="col-sm-5">
                    <b>Durée:</b>
                    <?php 
                        $date1 = new DateTime($item->rollback_transfer_date);
                        $date2 = new DateTime($item->transfer_date);
                        $interval = $date1->diff($date2);
                        $diffInDays = $interval->days;
                        echo $diffInDays ."Jours";
                      ?>
                    </div>
                  </div>
                </div>
            </div>
        </div>
    </div>
  <?php } ?>
  
<?php } ?>

<!-- Modal BOA -->
<?php if (!empty($ci)) { ?>     
  <div class="modal fade" id="myModalBoa<?php echo $ci[0]->id_nonau; ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
      <div class="modal-dialog" role="document">
          <div class="modal-content">
              <div class="modal-header">
                  <h6 class="text-center">Infos</h6>
                  <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                      <span aria-hidden="true">&times;</span>
                  </button>
              </div>
              <div class="modal-body">
                <div class="row">
                  <div class="col-sm-7">
                    <b>Régularisé</b>: 
                    <?php
                      $dateObj = new DateTime($ci[0]->date_regul);
                      $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                      $formattedDate2 = $formatter2->format($dateObj);
                    
                    echo "le" .$formattedDate2 ?>
                    
                  </div>
                  <div class="col-sm-5">
                  <b>Réf: </b><?php echo $ci[0]->ref_igor; ?> </p>
                  </div>
                  <div class="col-sm-7">
                    <b>opération</b>: <?php echo $ci[0]->oper; ?> </p>
                  </div>
                  <div class="col-sm-5">
                  <b>Durée:</b>
                  <?php 
                      $date1 = new DateTime($ci[0]->DATE_OPER);
                      $date2 = new DateTime($ci[0]->date_regul);
                      $interval = $date1->diff($date2);
                      $diffInDays = $interval->days;
                      echo $diffInDays ."Jours";
                    ?>
                  </div>
                  <div class="col-sm-7">
                  <b> Montant: </b>
                  <?php 
                      echo $ci[0]->montant ." MGA";
                    ?>
                  </div>
                  <div class="col-sm-5">
                  <b> Exploitant: </b>
                  <?php 
                      echo $ci[0]->expl;
                  ?>
                  </div>
                </div>
              </div>
          </div>
      </div>
  </div>
<?php } ?>

<!-- Modal Deallo -->

<?php if (!empty($deallo_vi)) { ?>
  <?php foreach($deallo_vi as $item) { ?>
   
    <div class="modal fade" id="myModalDeallo<?php echo $item->REF_IGOR; ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
        <div class="modal-dialog" role="document">
            <div class="modal-content">
                <div class="modal-header">
                    <h6 class="text-center">Infos</h6>
                    <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                        <span aria-hidden="true">&times;</span>
                    </button>
                </div>
                <div class="modal-body">
                  <?php foreach($deallo_vi as $item) { ?>
                    <div class="row">
                    <div class="col-sm-7">
                      <b>Régularisé</b>: <?php echo $item->TRANSFER_ID;  ?>
                    </div>
                    <div class="col-sm-5">
                      <b>Service: </b>: <?php echo $item->service_name;  ?></p>
                    </div>
                    <div class="col-sm-7">
                      <b>Réference</b>: <?php echo $item->reference_number;  ?></p>
                    </div>
                    <div class="col-sm-5">
                    <b>Durée:</b>
                   
                    </div>
                    <div class="col-sm-7">
                      <b> Montant: </b> <?php echo $item->amount;  ?>
                    </div>
                    <div class="col-sm-5">
                    <b> Exploitant: </b>
                    </div>
                  </div>
                  <?php } ?>

                  
                </div>
            </div>
        </div>
    </div>
  <?php } ?>
<?php } ?>

<script>
  let minDateBoa, maxDateBoa;
    DataTable.ext.search.push(function (settings, data, dataIndex) {
        let minBoa = minDateBoa.val();
        let max = maxDateBoa.val();
        let date = new Date(data[1]);
    
        if (
            (minBoa === null && max === null) ||
            (minBoa === null && date <= max) ||
            (minBoa <= date && max === null) ||
            (minBoa <= date && date <= max)
        ) {
            return true;
        }
        return false;
    });
    
    // Create date inputs
    minDateBoa = new DateTime('#minBoa', {
        format: 'LL', 
        locale: 'fr', 
    });

    maxDateBoa = new DateTime('#maxBoa', {
        format: 'LL', 
        locale: 'fr', 
    });
    
    // DataTables initialisation
    let tableBoa = new DataTable('#tableBoa', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
    });
    document.querySelectorAll('#minBoa, #maxBoa').forEach((el) => {
        el.addEventListener('change', () => tableBoa.draw());
    });
</script>

<script>
  let minDateAirtel, maxDateAirtel;
    DataTable.ext.search.push(function (settings, data, dataIndex) {
        let minAirtel = minDateAirtel.val();
        let maxAirtel = maxDateAirtel.val();
        let date = new Date(data[1]);
    
        if (
            (minAirtel === null && maxAirtel === null) ||
            (minAirtel === null && date <= maxAirtel) ||
            (minAirtel <= date && maxAirtel === null) ||
            (minAirtel <= date && date <= maxAirtel)
        ) {
            return true;
        }
        return false;
    });
    
    // Create date inputs
    minDateAirtel = new DateTime('#minAirtel', {
        format: 'LL', 
        locale: 'fr', 
    });

    maxDateAirtel = new DateTime('#maxAirtel', {
        format: 'LL', 
        locale: 'fr', 
    });
    
    // DataTables initialisation
    let tableAirtel = new DataTable('#tableAirtel', {
        scrollX: true,
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
            }
    });
    document.querySelectorAll('#minAirtel, #maxAirtel').forEach((el) => {
        el.addEventListener('change', () => tableAirtel.draw());
    });
</script>
<script>



$("#dat").DataTable({
    scrollX: true,
    autoFill: true,
    dom: 'Bfrtip',
    buttons: [
      'copy', 'csv', 'excel', 'pdf', 'print'
    ],
    language: {
        url: '<?php echo base_url(); ?>assets/fr-FR.json',
    }
  });

    // Formatting function for row details - modify as you need
// function format(d) {
//     // `d` is the original data object for the row
//     return (
//         '<dl>' +
//         '<dt>Full name:</dt>' +
//         '<dd>' +
//         d.name +
//         '</dd>' +
//         '<dt>Extension number:</dt>' +
//         '<dd>' +
//         d.extn +
//         '</dd>' +
//         '<dt>Extra info:</dt>' +
//         '<dd>And any further details here (images etc)...</dd>' +
//         '</dl>'
//     );
// }
 
// let table = new DataTable('#dat', {
//     ajax: {
//         url: '<?php echo base_url();?>get_data', 
//         type: 'GET',
//         dataType: 'json',
//     },
//     columns: [
//         {
//             className: 'dt-control',
//             orderable: false,
//             data: null,
//             defaultContent: ''
//         },
//         { data: 'name' },
//         { data: 'position' },
//         { data: 'office' },
//         { data: 'salary' }
//     ],
//     order: [[1, 'asc']],
//     language: {
//             url: '<?php echo base_url(); ?>assets/fr-FR.json',
//         }
// },
// );
 
// Add event listener for opening and closing details
// table.on('click', 'td.dt-control', function (e) {
//     let tr = e.target.closest('tr');
//     let row = table.row(tr);
 
//     if (row.child.isShown()) {
//         // This row is already open - close it
//         row.child.hide();
//     }
//     else {
//         // Open this row
//         row.child(format(row.data())).show();
//     }
// });

</script>







  <script>

    




    // function format(d) {

    //     return (
    //         '<dl>' +
    //         '<dt>Full name:</dt>' +
    //         '<dd>' +
    //         d.name +
    //         '</dd>' +
    //         '<dt>Extension number:</dt>' +
    //         '<dd>' +
    //         d.extn +
    //         '</dd>' +
    //         '<dt>Extra info:</dt>' +
    //         '<dd>And any further details here (images etc)...</dd>' +
    //         '</dl>'
    //     );
    // }

    // let table= new DataTable("#tableDat", {

    //     // ajax: '../ajax/data/objects.txt',
    //     columns: [
    //         {
    //             className: 'dt-control',
    //             orderable: false,
    //             data: null,
    //             defaultContent: ''
    //         },
    //         {data: "Compte"},
    //         {data: "Date_Oper"},
    //         {data: "Date_val"},
    //         {data: "Devise"},
    //         {data: "Montant"},
    //         {data: "Libelle"},
    //         {data: "Operation"},
    //         {data: "Expl"},
    //         {data: "ReferenceIgor"}
    //     ],
    //     order: [[1, 'asc']]
    //     }
    // );

    // table.on('click', 'td.dt-control', function (e) {
    //     let tr = e.target.closest('tr');
    //     let row = table.row(tr);
    //     if (row.child.isShown()) {
    //         row.child.hide();
    //     }
    //     else {
    //         row.child(format(row.data())).show();
    //     }
    // });
  </script>