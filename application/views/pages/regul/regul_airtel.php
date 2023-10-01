<div id="anormale" class="mt-5 container"><br>
    <ul class="nav nav-tabs">
    <li class="nav-item col-sm-3">
        <a class="nav-link" href="#tabDat">Débit à Tort</a>
    </li>
    <li class="nav-item col-sm-3">
        <a class="nav-link" href="#tabCat">Crédit à Tort</a>
    </li>
    <li class="nav-item col-sm-3">
        <a class="nav-link" href="#tabAmbi">Ambiguous</a>
    </li>
    <li class="nav-item col-sm-2">
        <a class="nav-link" href="#tabDeallo">Deallocation</a>
    </li>
    <li class="nav-item col-sm-1">
        <a class="nav-link" href="#tabVI">VI</a>
    </li>
    </ul>
</div>

<!-- <div id="tabDat" class="container tab-pane "><br>   
    <div class="table-responsive">
      <table class="table table-bordered" id="tableDat">
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
            <tr>
              <td></td>
              <td></td>
              <td> </td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td></td>
              <td><a href="#" class="btn btn-info" data-toggle="modal" data-target="#myModal" ></a></td>
            </tr>
        </tbody>
      </table>
    </div>
  </div>
  <form id="modifForm">

  <div class="modal fade" id="myModal">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="card p-3">
          <div class="text-center" style="font-size: 17px;">
            <b>Régulariser</b>
            <button type="button" class="close" data-dismiss="modal">&times;</button>
          </div>
            <div class="row form-group mt-2">
                <div class="col-sm-6" style="display: none;">
                  <label for="ref">ref</label>
                  <input type="text" class="form-control" id="ref_igor" name="ref_igor">
                </div>
                <div class="col-sm-6">
                  <label for="etat">Etat</label>
                  <select name="etat" id="etat" class="form-control">
                    <option value="Non">Non</option>
                    <option value="En cours">En cours</option>
                  </select>
                </div>
                <div class="col-sm-6">
                  <label for="date">Date</label>
                  <input type="date" class="form-control" id="date_regul" name="date_regul" required>
                </div>
                </div>
                <div class="text-center">
                  <button type="button" class="btn btn-danger" data-dismiss="modal">Annuler</button>
                  <input type="submit" class="btn btn-success" value="Valider" />
                </div>
              </div>
            </div>
      </div>
    </div>
  </div>
  
</form> -->



<div class="container mt-5">
  <div id="tabDat">
    <div class="table-responsive">
    <table id="dat" class="display" style="width:100%">
        <thead>
          <tr>
            <th>Date_Oper</th>
            <th>Date_Val </th>
            <th>Devise</th>
            <th>Montant</th>
            <th>Libelle</th>
            <th>Operation</th>
            <th>Expl</th>
            <th>ReferenceIgor</th>
            <th>Date_régul</th>
            <th>Action</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach($ci as $item)  { ?>
            <tr>
              
              <td><?php echo $item->DATE_OPER; ?></td>
              <td><?php echo $item->DATE_VAL; ?></td>
              <td><?php echo $item->MONTANT; ?></td>
              <td><?php echo $item->DEVISE; ?></td>
              <td style=" overflow: hidden; white-space: nowrap;"><?php echo $item->LIBELLE; ?></td>
              <td><?php echo $item->OPER; ?></td>
              <td><?php echo $item->EXPL; ?></td>
              <td><?php echo $item->REF_IGOR; ?></td>
              <td><?php echo $item->date_regul; ?></td>
              <td><a href="#" data-toggle="modal" data-target="#myModal<?php echo $item->id; ?>">Afficher Infos</a></td>
            </tr>
          <?php } ?>
        </tbody>
    </table>

    </div>
  </div>
    
</div>

<div class="container mt-5">
  <div id="tabCat">
    <div class="table-responsive">
    <table id="dat" class="display" style="width:100%">
        <thead>
          <tr>
            <th>TRANSFER_ID</th>
            <th>transfer_date </th>
            <th>external_id</th>
            <th>account_no</th>
            <th>sender_msisdn</th>
            <th>dest_msisdn</th>
            <th>amount</th>
            <th>solde</th>
            <th>description</th>
            <th>service_name</th>
            <th>reference_number</th>
          </tr>
        </thead>
        <tbody>
          <?php foreach($vi as $item)  { ?>
            <tr>
              
              <td><?php echo $item->TRANSFER_ID; ?></td>
              <td><?php echo $item->transfer_date; ?></td>
              <td><?php echo $item->external_id; ?></td>
              <td><?php echo $item->account_no; ?></td>
              <td style=" overflow: hidden; white-space: nowrap;"><?php echo $item->sender_msisdn; ?></td>
              <td><?php echo $item->dest_msisdn; ?></td>
              <td><?php echo $item->amount; ?></td>
              <td><?php echo $item->solde; ?></td>
              <td><?php echo $item->description; ?></td>
              <td><?php echo $item->service_name; ?></td>
              <td><?php echo $item->reference_number; ?></td>
              <td><a href="#" data-toggle="modal" data-target="#myModal<?php echo $item->id; ?>">Afficher Infos</a></td>
            </tr>
          <?php } ?>
        </tbody>
    </table>

    </div>
  </div>
    
</div>

<div class="modal fade" id="myModal<?php echo $item->id; ?>" tabindex="-1" role="dialog" aria-labelledby="myModalLabel">
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
                  <b>Régulariser</b>: 
                  <?php
                    $dateObj = new DateTime($item->date_regul);
                    $formatter2 = new IntlDateFormatter('fr_FR', IntlDateFormatter::LONG, IntlDateFormatter::NONE);
                    $formattedDate2 = $formatter2->format($dateObj);
                  
                  echo "le" .$formattedDate2 ?>
                  
                </div>
                <div class="col-sm-5">
                <b>Réf: </b><?php echo $item->ref_igor; ?> </p>
                </div>
                <div class="col-sm-7">
                  <b>opération</b>: <?php echo $item->oper; ?> </p>
                </div>
                <div class="col-sm-5">
                <b>Durée:</b>
                <?php 
                    $date1 = new DateTime($item->DATE_VAL);
                    $date2 = new DateTime($item->date_regul);
                    $interval = $date1->diff($date2);
                    $diffInDays = $interval->days;
                    echo $diffInDays ."Jours";
                  ?>
                </div>
                <div class="col-sm-7">
                <b> Montant: </b>
                <?php 
                    echo $item->montant ." MGA";
                  ?>
                </div>
                <div class="col-sm-5">
                <b> Exploitant: </b>
                <?php 
                    echo $item->expl;
                ?>
                </div>
              </div>
            </div>
        </div>
    </div>
</div>




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