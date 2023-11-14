

    <div id="tabCat" class="container-fluid mt-1 tab-pane "><br>
      <div class="row">
          <div class="col-sm-6 pl-4">
              <label for="dateDebut">Date Debut</label>
              <input type="text" class="form-control" id="minBoa" name="minBoa">
          </div>
          <div class="col-sm-6 pr-4">
            <label for="dateDebut">Date Fin</label>
              <input type="text" class="form-control" id="maxBoa" name="maxBoa">
          </div> 
      </div>
      <div class="container-fluid mt-4">
        <div class="card">
          <div class="card-header">
            <b>Liste Credit à tort Regularisé</b>
          </div>
          <div class="card-body">
            <table class="table table-bordered" id="tableCat">
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
                <?php if (!empty($co)) { ?>
                  <?php foreach($co as $item)  { ?>
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
                      <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalCat<?php echo $item->id_airtel; ?>"><i class="bx bx-show" style="font-size: 20px"></i></a></td>
                    </tr>
                  <?php } ?>
                <?php } ?>
              </tbody>
            </table>
          </div>
        </div>
      </div>
    </div>

    <?php $this->load->view("pages/regul/airtel/modal/modalCat")?>