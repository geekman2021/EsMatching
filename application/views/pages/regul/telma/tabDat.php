<div class="tab-content">
    <div id="tabDat" class="container-fluid mt-1 tab-pane "><br>  
        <!-- <div class="row">
            <div class="col-sm-6 pl-4">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="min" name="min">
            </div>
            <div class="col-sm-6 pr-4">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="max" name="max">
            </div> 
        </div>  -->
        <div class="container-fluid mt-5">
            <div class="card">
                <div class="card-header">
                    <b>Liste Debit à Tort regularisé</b>
                </div>
                <div class="card-body">
                <table class="table table-bordered" id="tableDat">
                <thead style="text-align: center ;">
                    <tr>
                        <th>Compte</th>
                        <th>Date_Oper</th>
                        <th>Date_Val </th>
                        <th>Montant</th>
                        <th>Devise</th>
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
              <td><?= $item->COMPTE ?></td>
              <td><?= $item->DATE_OPER ?></td>
              <td><?= $item->DATE_VAL ?></td>
              <td><?= $item->DEVISE ?></td>
              <td style="white-space: nowrap;"><?= number_format($item->MONTANT, 0, ',', ' ') ?></td>
              <td><?= $item->LIBELLE ?></td>
              <td><?= $item->OPER ?></td>
              <td><?= $item->EXPL ?></td>
              <td><?= $item->REF_IGOR ?></td>
              <td style="text-align: center;"><a href="#" class="btn btn-info"  data-toggle="modal" data-target="#myModalDat<?php echo $item->id_nonau; ?>"><i class="bx bx-show" style="font-size: 20px"></i></a></td>
            </tr>
          <?php } ?>
        <?php } ?>
                </tbody>
            </table>
                </div>
            </div>
           
        </div>   
    </div>
<?php $this->load->view("pages/regul/telma/modal/modalDat"); ?>



